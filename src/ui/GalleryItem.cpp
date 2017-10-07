#include <QtConcurrent>
#include <QGraphicsSceneMouseEvent>
#include <QPalette>
#include <QThreadPool>
#include <QVariant>

#include "ui/GalleryItem.h"
#include "ui/GalleryView.h"

const QColor GalleryItem::SELECTED_COLOR = QColor("#AAFFFFFF");

static QThreadPool *g_resizeThumbnailThreads = 0;
static QThreadPool *gResizeThumbnailThreads()
{
    if (!g_resizeThumbnailThreads) {
        g_resizeThumbnailThreads = new QThreadPool();
        g_resizeThumbnailThreads->setMaxThreadCount(QThread::idealThreadCount() - 1);
    }

    return g_resizeThumbnailThreads;
}

GalleryItem::GalleryItem(AbstractRendererFactory *rendererFactory,
                         QGraphicsItem *parent) :
    QGraphicsItem(parent) ,
    m_rendererFactory(rendererFactory) ,
    m_renderer(0) ,
    m_thumbnail(nullptr) ,
    // Hide until thumbnail is ready.
    // It will show during next view layout after thumbnail is loaded
    //
    // This strategy is to hide the crazy rapid layout procedure when loading
    // remote gallery in waterfall mode.
    m_isVisible(false) ,
    m_isReadyToShow(false) ,
    m_isInsideViewportPreload(false) ,
    m_selectedAfterShownScheduled(false)
{
}

GalleryItem::~GalleryItem()
{
    if (m_renderer) {
        delete m_renderer;
    }
}

QString GalleryItem::name() const
{
    return QString();
}

void GalleryItem::setRenderer(AbstractGalleryItemRenderer *renderer)
{
    delete m_renderer;
    m_renderer = renderer;

    // Hide until thumbnail scaled and ready to paint.
    hide();

    QImage *thumbnail = m_thumbnail.take();
    setThumbnail(thumbnail);
}

void GalleryItem::setRendererFactory(AbstractRendererFactory *factory)
{
    m_rendererFactory = factory;
    createRenderer();
}

void GalleryItem::setThumbnail(QImage *thumbnail)
{
    m_thumbnail.reset(thumbnail);

    m_renderer->setImage(m_thumbnail.data());
    if (m_thumbnail) {
        m_thumbnailSize = m_thumbnail->size();
    }
    m_renderer->setImageSize(m_thumbnailSize);
    m_renderer->layout();

    prepareGeometryChange();

    update(boundingRect());
    if (scene()) {
        emit requestLayout();
    }

    // We always consider it ready no matter whether thumbnail is null
    if (!m_isReadyToShow && thumbnail != nullptr) {
        m_isReadyToShow = true;
        emit readyToShow();
    }

    unload();

    if (!m_isInsideViewportPreload) {
        resetThumbnail();
    }
}

bool GalleryItem::isReadyToShow()
{
    return m_thumbnail != 0;
}

void GalleryItem::scheduleSelectedAfterShown()
{
    m_selectedAfterShownScheduled = true;
}

void GalleryItem::show()
{
    m_isVisible = true;
    onVisibilityChanged(m_isVisible);
    update(boundingRect());
}

void GalleryItem::hide()
{
    m_isVisible = false;
    onVisibilityChanged(m_isVisible);
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
    painter->setBackground(scene()->palette().background());
    painter->setBrush(scene()->palette().foreground());

    // Clear painter first
    painter->eraseRect(boundingRect());

    if (!m_isVisible) {
        return;
    }

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
    m_isInsideViewportPreload = isInside;

    // // if (m_isInsideViewportPreload && !isInside) {
    if (!isInside) {
        // Unload thumbnail
        resetThumbnail();
        unload();
        // update(boundingRect());
    }

    // if (!m_isInsideViewportPreload && isInside) {
    if (isInside) {
        load();
        // show();
    }
}

void GalleryItem::resetThumbnail()
{
    m_thumbnail.reset();
    m_renderer->setImage(nullptr);
}
