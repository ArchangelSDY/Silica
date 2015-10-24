#include "D2DMainGraphicsWidget.h"

#include "DeviceResources.h"
#include "DirectXHelper.h"

#include <QTransform>
#include <QWheelEvent>

D2DMainGraphicsWidget::D2DMainGraphicsWidget(QWidget *parent) : QWidget(parent) ,
    m_deviceResources(new DX::DeviceResources()) ,
    m_isViewDirty(false)
{
    // TODO: Device lost
    m_deviceResources->SetSwapWidget(this);

    createDeviceIndependentResources();
    createDeviceDependentResources();

    m_drawTimer.setInterval(20);
    connect(&m_drawTimer, SIGNAL(timeout()), this, SLOT(checkDraw()));
}

QPaintEngine *D2DMainGraphicsWidget::paintEngine() const
{
    return 0;
}

void D2DMainGraphicsWidget::setImage(const QImage &image)
{
    auto d2dContext = m_deviceResources->GetD2DDeviceContext();

    D2D1_SIZE_U size = D2D1::SizeU(image.width(), image.height());

    D2D1_BITMAP_PROPERTIES1 props{
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE),
        FLOAT(96.0f * image.devicePixelRatio()),
        FLOAT(96.0f * image.devicePixelRatio())
    };

    m_imageBitmap.Reset();

    if (FAILED(d2dContext->CreateBitmap(
        size,
        image.bits(),
        image.bytesPerLine(),
        &props,
        &m_imageBitmap
        ))) {
        return;
    }

    m_transformEffect->SetInput(0, m_imageBitmap.Get());

    scheduleDraw();
}

void D2DMainGraphicsWidget::setViewportRect(const QRect &rect)
{
    m_viewportRect = rect;
    m_center = m_viewportRect.center();
}

void D2DMainGraphicsWidget::setTransform(const QTransform &transform)
{
    m_transform = transform;

    D2D1_MATRIX_3X2_F mat{
        FLOAT(transform.m11()), FLOAT(transform.m12()),
        FLOAT(transform.m21()), FLOAT(transform.m22()),
        FLOAT(transform.m31()), FLOAT(transform.m32())
    };

    m_transformEffect->SetValue(D2D1_2DAFFINETRANSFORM_PROP_TRANSFORM_MATRIX, mat);

    scheduleDraw();
}

void D2DMainGraphicsWidget::setFitInView(const QRectF &rect, Qt::AspectRatioMode aspectRatioMode)
{
    if (rect.isNull()) {
        return;
    }

    // Reset the view scale to 1:1.
    QRectF unity = m_transform.mapRect(QRectF(0, 0, 1, 1));
    if (unity.isEmpty()) {
        return;
    }
    scale(1 / unity.width(), 1 / unity.height());

    // Find the ideal x / y scaling ratio to fit \a rect in the view.
    int margin = 2;
    QRectF viewRect = this->rect();
    QRectF mappedTargetRect = m_transform.mapRect(rect);
    if (mappedTargetRect.isEmpty()) {
        return;
    }
    qreal xratio = viewRect.width() / mappedTargetRect.width();
    qreal yratio = viewRect.height() / mappedTargetRect.height();

    // Respect the aspect ratio mode.
    switch (aspectRatioMode) {
    case Qt::KeepAspectRatio:
        xratio = yratio = qMin(xratio, yratio);
        break;
    case Qt::KeepAspectRatioByExpanding:
        xratio = yratio = qMax(xratio, yratio);
        break;
    case Qt::IgnoreAspectRatio:
        break;
    }

    // Scale and center on the center of \a rect.
    scale(xratio, yratio);

    centerOn(rect.center());

    scheduleDraw();
}

void D2DMainGraphicsWidget::centerOn(const QPointF &center)
{
    m_center = center;
    scheduleDraw();
}

void D2DMainGraphicsWidget::scale(qreal sx, qreal sy)
{
    QTransform transform = m_transform;
    transform.scale(sx, sy);
    setTransform(transform);
}

// Scroll viewport
// Note that dx, dy are relative to actual view.
void D2DMainGraphicsWidget::scroll(qreal dx, qreal dy)
{
    QRectF mappedViewport = m_transform.mapRect(m_viewportRect);
    QPointF mappedCenter = m_transform.map(m_center);

    qreal mappedLeft = width() / 2 - mappedCenter.x();
    qreal mappedRight = mappedLeft + mappedViewport.width();
    qreal mappedTop = height() / 2 - mappedCenter.y();
    qreal mappedBottom = mappedTop + mappedViewport.height();

    if (mappedViewport.width() > width()) {
        if (mappedLeft + dx > 0) {
            dx = -mappedLeft;
        }
        if (mappedRight + dx < width()) {
            dx = width() - mappedRight;
        }
    } else {
        dx = 0;
    }

    if (mappedViewport.height() > height()) {
        if (mappedTop + dy > 0) {
            dy = -mappedTop;
        }
        if (mappedBottom + dy < height()) {
            dy = height() - mappedBottom;
        }
    } else {
        dy = 0;
    }

    QPointF mov(dx, dy);
    QTransform invt = m_transform.inverted();
    QPointF origMov = invt.map(mov);
    QPointF center = m_center - origMov;

    centerOn(center);
}

bool D2DMainGraphicsWidget::isVisible() const
{
    return QWidget::isVisible();
}

QSize D2DMainGraphicsWidget::viewSize() const
{
    return QWidget::size();
}

QRectF D2DMainGraphicsWidget::viewportRect() const
{
    return m_viewportRect;
}

QTransform D2DMainGraphicsWidget::transform() const
{
    return m_transform;
}

QWidget *D2DMainGraphicsWidget::widget()
{
    return this;
}

void D2DMainGraphicsWidget::showEvent(QShowEvent *ev)
{
    m_model->showEvent(ev);

    draw();
}

void D2DMainGraphicsWidget::wheelEvent(QWheelEvent *ev)
{
    m_model->wheelEvent(ev);

    if (!ev->isAccepted()) {
        QWidget::wheelEvent(ev);
    }
}

void D2DMainGraphicsWidget::resizeEvent(QResizeEvent *ev)
{
    m_model->resizeEvent(ev);

    m_deviceResources->SetLogicalSize(size());

    draw();
}

void D2DMainGraphicsWidget::keyPressEvent(QKeyEvent *ev)
{
    m_model->keyPressEvent(ev);

    if (!ev->isAccepted()) {
        QWidget::keyPressEvent(ev);
    }
}

void D2DMainGraphicsWidget::mouseMoveEvent(QMouseEvent *ev)
{
    m_model->mouseMoveEvent(ev);
    if (ev->isAccepted()) {
        return;
    }

    if (m_isDragging) {
        qreal dx = ev->pos().x() - m_lastDragPos.x();
        qreal dy = ev->pos().y() - m_lastDragPos.y();

        scroll(dx, dy);

        m_lastDragPos = ev->pos();
    }

    QWidget::mouseMoveEvent(ev);
}

void D2DMainGraphicsWidget::mousePressEvent(QMouseEvent *ev)
{
    m_model->mousePressEvent(ev);
    if (ev->isAccepted()) {
        return;
    }

    m_isDragging = true;
    m_lastDragPos = ev->pos();
    setCursor(Qt::ClosedHandCursor);

    QWidget::mousePressEvent(ev);
}

void D2DMainGraphicsWidget::mouseReleaseEvent(QMouseEvent *ev)
{
    m_isDragging = false;
    setCursor(Qt::ArrowCursor);

    QWidget::mouseReleaseEvent(ev);
}

void D2DMainGraphicsWidget::mouseDoubleClickEvent(QMouseEvent *ev)
{
    m_model->mouseDoubleClickEvent(ev);
}

void D2DMainGraphicsWidget::contextMenuEvent(QContextMenuEvent *ev)
{
    m_model->contextMenuEvent(ev);
}

void D2DMainGraphicsWidget::createDeviceIndependentResources()
{
}

void D2DMainGraphicsWidget::createDeviceDependentResources()
{
    auto d2dContext = m_deviceResources->GetD2DDeviceContext();

    // Create the photo adjustment pipeline.
    DX::ThrowIfFailed(d2dContext->CreateEffect(CLSID_D2D12DAffineTransform, &m_transformEffect));

    // m_outputEffect always points to the photo pipeline output.
    DX::ThrowIfFailed(m_transformEffect.CopyTo(&m_outputEffect));

    // Transform effect
    D2D1_MATRIX_3X2_F matrix = D2D1::Matrix3x2F::Identity();
    m_transformEffect->SetValue(D2D1_2DAFFINETRANSFORM_PROP_TRANSFORM_MATRIX,
                                matrix);
    m_transformEffect->SetValue(D2D1_2DAFFINETRANSFORM_PROP_INTERPOLATION_MODE,
                                D2D1_2DAFFINETRANSFORM_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC);
}

QPointF D2DMainGraphicsWidget::drawOffset() const
{
    QPointF mappedCenter = m_transform.map(m_center);
    QPointF topLeft(-mappedCenter.x() + width() / 2,
                    -mappedCenter.y() + height() / 2);
    return topLeft;
}

void D2DMainGraphicsWidget::scheduleDraw()
{
    m_isViewDirty = true;
    if (!m_drawTimer.isActive()) {
        m_drawTimer.start();
    }
}

void D2DMainGraphicsWidget::checkDraw()
{
    if (m_isViewDirty) {
        draw();
        m_isViewDirty = false;
    }
    m_drawTimer.stop();
}

void D2DMainGraphicsWidget::draw()
{
    if (m_imageBitmap.Get() == 0) {
        return;
    }

    auto d2dContext = m_deviceResources->GetD2DDeviceContext();

    d2dContext->BeginDraw();

    // DeviceResources explicitly handles screen rotation by setting the rotation on the swap chain.
    // In order for Direct2D content to appear correctly on this swap chain, the app is responsible for
    // explicitly "prerotating" by setting the appropriate transform before drawing.
    // 
    // TODO: Device rotation
    // m_deviceResources->GetD2DDeviceContext()->SetTransform(
    //     m_deviceResources->GetOrientationTransform2D());
    d2dContext->Clear(D2D1::ColorF(0x323A44, 1.0f));

    QPointF offset = drawOffset();
    D2D1_POINT_2F imageOffset = D2D1::Point2F(offset.x(), offset.y());

    d2dContext->DrawImage(m_outputEffect.Get(), &imageOffset);

    // We ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
    // is lost. It will be handled during the next call to Present.
    HRESULT hr = d2dContext->EndDraw();
    if (hr != D2DERR_RECREATE_TARGET)
    {
        DX::ThrowIfFailed(hr);
    }

    m_deviceResources->Present();
}

