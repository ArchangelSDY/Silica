#ifndef D2DMAINGRAPHICSWIDGET_H
#define D2DMAINGRAPHICSWIDGET_H

#include <QSharedPointer>
#include <QWidget>

#include "DeviceResources.h"

#include "ui/models/MainGraphicsViewModel.h"

class D2DMainGraphicsWidget : public QWidget, public MainGraphicsViewModel::View
{
    Q_OBJECT
public:
    explicit D2DMainGraphicsWidget(QWidget *parent = 0);

    QPaintEngine *paintEngine() const;

    virtual bool isVisible() const;

protected:
    virtual void showEvent(QShowEvent *event);
    virtual void wheelEvent(QWheelEvent *);
    virtual void resizeEvent(QResizeEvent *);
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseDoubleClickEvent(QMouseEvent *);
    virtual void contextMenuEvent(QContextMenuEvent *event);

private slots:
    void checkDraw();

private:
    virtual void setImage(const QImage &image);
    virtual void setViewportRect(const QRect &rect);
    virtual void setTransform(const QTransform &transform);
    virtual void setFitInView(const QRectF & rect, Qt::AspectRatioMode aspectRatioMode = Qt::IgnoreAspectRatio);
    virtual QSize viewSize() const;
    virtual QRectF viewportRect() const;
    virtual QTransform transform() const;
    virtual QWidget *widget();

    void centerOn(const QPointF &center);
    void scale(qreal sx, qreal sy);
    void scroll(qreal dx, qreal dy);
    virtual void scroll(int dx, int dy);

    QPointF drawOffset() const;

    void createDeviceIndependentResources();
    void createDeviceDependentResources();
    // void createWindowSizeDependentResouces();

    void scheduleDraw();
    void draw();

    QSharedPointer<DX::DeviceResources> m_deviceResources;

    QRect m_viewportRect;
    QPointF m_center;
    QTransform m_transform;
    bool m_isViewDirty;
    QTimer m_drawTimer;
    QPointF m_lastDragPos;
    bool m_isDragging;

    Microsoft::WRL::ComPtr<ID2D1Bitmap1> m_imageBitmap;

    Microsoft::WRL::ComPtr<ID2D1Effect>  m_transformEffect;
    Microsoft::WRL::ComPtr<ID2D1Effect>  m_outputEffect;
};

#endif // D2DMAINGRAPHICSWIDGET_H
