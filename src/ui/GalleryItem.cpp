#include <QtConcurrent>
#include <QGraphicsSceneMouseEvent>
#include <QPalette>
#include <QThreadPool>
#include <QVariant>

#include "ui/GalleryItem.h"
#include "ui/GalleryView.h"

const QColor GalleryItem::SELECTED_COLOR = QColor("#AAFFFFFF");

static QThreadPool *g_scaleThumbnailThreads = 0;
static QThreadPool *gScaleThumbnailThreads()
{
    if (!g_scaleThumbnailThreads) {
        g_scaleThumbnailThreads = new QThreadPool();
        g_scaleThumbnailThreads->setMaxThreadCount(QThread::idealThreadCount() - 1);
    }

    return g_scaleThumbnailThreads;
}

GalleryItem::GalleryItem(AbstractRendererFactory *rendererFactory,
                         QGraphicsItem *parent) :
    QGraphicsItem(parent) ,
    m_rendererFactory(rendererFactory) ,
    m_renderer(0) ,
    m_thumbnailScaled(nullptr) ,
    m_isReadyToShow(false) ,
    m_isInsideViewportPreload(false) ,
    m_selectedAfterShownScheduled(false)
{
    connect(&m_thumbnailScaleWatcher, &QFutureWatcher<QSharedPointer<QImage> >::finished,
            this, &GalleryItem::onThumbnailScaled);
}

GalleryItem::~GalleryItem()
{
}

QString GalleryItem::name() const
{
    return QString();
}

void GalleryItem::setRenderer(AbstractGalleryItemRenderer *renderer)
{
    m_renderer.reset(renderer);

    // Hide until thumbnail scaled and ready to paint.
    hide();
}

void GalleryItem::setRendererFactory(AbstractRendererFactory *factory)
{
    m_rendererFactory = factory;
    createRenderer();
    load();
}

AbstractGalleryItemRenderer *GalleryItem::renderer() const
{
    return m_renderer.data();
}

static QSharedPointer<QImage> scaleThumbnailAtBackground(QSharedPointer<QImage> thumbnail,
                                                          const QSize &size,
                                                          Qt::AspectRatioMode aspectRatioMode)
{
    return QSharedPointer<QImage>::create(std::move(thumbnail->scaled(size, aspectRatioMode, Qt::SmoothTransformation)));
}

static bool thumbnailNeedResize(const QSize &thumbnailSize, const QSize &targetSize)
{
    return thumbnailSize.width() > targetSize.width() * 2 || thumbnailSize.height() > targetSize.height() * 2
        || targetSize.width() > thumbnailSize.width() * 2 || targetSize.height() > thumbnailSize.height() * 2;
}

void GalleryItem::setThumbnail(QSharedPointer<QImage> thumbnail)
{
    if (thumbnail) {
        // Layout here first using original thumbnail size,
        // which is required by some view renderers
        m_renderer->setImageSize(thumbnail->size());
        if (scene()) {
            m_renderer->layout(scene()->sceneRect().toRect());
        }

        auto boundingSize = boundingRect().size().toSize();
        if (thumbnailNeedResize(thumbnail->size(), boundingSize)) {
            auto scaleFuture = QtConcurrent::run(
                // gScaleThumbnailThreads(),
                scaleThumbnailAtBackground,
                thumbnail,
                boundingRect().size().toSize(),
                m_renderer->aspectRatioMode()
            );
            m_thumbnailScaleWatcher.setFuture(scaleFuture);
        } else {
            setScaledThumbnail(thumbnail);
        }

    } else {
        resetThumbnail();
    }
}

void GalleryItem::onThumbnailScaled()
{
    setScaledThumbnail(m_thumbnailScaleWatcher.result());
}

void GalleryItem::setScaledThumbnail(QSharedPointer<QImage> thumbnailScaled)
{
    m_thumbnailScaled = thumbnailScaled;
    m_thumbnailSize = m_thumbnailScaled->size();

    // Layout again using scaled thumbnail size
    m_renderer->setImage(m_thumbnailScaled);
    m_renderer->setImageSize(m_thumbnailSize);
    if (scene()) {
        m_renderer->layout(scene()->sceneRect().toRect());
    }

    prepareGeometryChange();

    update(boundingRect());
    if (scene()) {
        emit requestLayout();
    }

    // We always consider it ready no matter whether thumbnail is null
    if (!m_isReadyToShow) {
        m_isReadyToShow = true;
        emit readyToShow();
    }

    // Clear thumbnail if currently this item is not in preload area
    if (!m_isInsideViewportPreload) {
        resetThumbnail();
    }
}

bool GalleryItem::isReadyToShow()
{
    return m_thumbnailScaled != 0;
}

void GalleryItem::scheduleSelectedAfterShown()
{
    m_selectedAfterShownScheduled = true;
}

void GalleryItem::show()
{
    onVisibilityChanged(true);
    update(boundingRect());
}

void GalleryItem::hide()
{
    onVisibilityChanged(false);
    update(boundingRect());
}

void GalleryItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        event->accept();
        return;
    }

    QGraphicsItem::mousePressEvent(event);
}

void GalleryItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    emit mouseDoubleClicked();
    QGraphicsItem::mouseDoubleClickEvent(event);
}

void GalleryItem::paint(QPainter *painter,
                        const QStyleOptionGraphicsItem *option,
                        QWidget *widget)
{
    painter->setRenderHints(
        QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    painter->setPen(Qt::NoPen);
    painter->setBackground(scene()->palette().window());
    painter->setBrush(scene()->palette().windowText());

    // Clear painter first
    painter->eraseRect(boundingRect());


    // Render
    m_renderer->paint(painter);

    // Draw a mask if selected
    if (isSelected()) {
        painter->fillRect(boundingRect(), GalleryItem::SELECTED_COLOR);
    }
}

void GalleryItem::onVisibilityChanged(bool isVisible)
{
    if (isVisible && m_selectedAfterShownScheduled) {
        setSelected(true);
        m_selectedAfterShownScheduled = false;
    }
}

void GalleryItem::setIsInsideViewportPreload(bool isInside)
{
    if (m_isInsideViewportPreload != isInside) {
        // Must be assigned before `load()` as `load()` might be a sync call if thumbnail doesn't need scale
        m_isInsideViewportPreload = isInside;
        if (!m_isInsideViewportPreload) {
            resetThumbnail();
        } else {
            load();
        }
    }
}

void GalleryItem::resetThumbnail()
{
    m_thumbnailScaled.reset();
    m_renderer->setImage(nullptr);
}