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
    m_thumbnailScaled(nullptr) ,
    // Hide until thumbnail is ready.
    // It will show during next view layout after thumbnail is loaded
    //
    // This strategy is to hide the crazy rapid layout procedure when loading
    // remote gallery in waterfall mode.
    m_isVisible(false) ,
    m_isInsideViewportPreload(false) ,
    m_selectedAfterShownScheduled(false)
{
    connect(&m_thumbnailResizeWatcher, SIGNAL(finished()), this, SLOT(onThumbnailResized()));
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
    qDebug("set renderer hide");
    hide();

    QImage *thumbnail = m_thumbnail.take();
    setThumbnail(thumbnail);
}

void GalleryItem::setRendererFactory(AbstractRendererFactory *factory)
{
    m_rendererFactory = factory;
    createRenderer();
}

static QSharedPointer<QImage> resizeThumbnailAtBackground(QSharedPointer<QImage> thumbnail,
                                                          const QSize &size,
                                                          Qt::AspectRatioMode aspectRatioMode)
{
    return QSharedPointer<QImage>::create(thumbnail->scaled(size, aspectRatioMode, Qt::SmoothTransformation));
}

void GalleryItem::setThumbnail(QImage *thumbnail)
{
    m_thumbnail.reset(thumbnail);

    // Some renderer depends on thumbnail aspect ratio to determine bounding rect.
    // So before making resized thumbnail image, we layout once using original image size.
    m_renderer->setImage(nullptr);
    m_renderer->setImageSize(m_thumbnail ? m_thumbnail->size() : QSize());
    m_renderer->layout();

    if (thumbnail) {
        qDebug() << "thumbnail not null will resize";
        QFuture<QSharedPointer<QImage> > resizeFuture = QtConcurrent::run(
            gResizeThumbnailThreads(), resizeThumbnailAtBackground,
            QSharedPointer<QImage>::create(*thumbnail), boundingRect().size().toSize(),
            m_renderer->aspectRatioMode());
        m_thumbnailResizeWatcher.setFuture(resizeFuture);
    } else {
        qDebug() << "thumbnail null wont resize";
    }
}

void GalleryItem::onThumbnailResized()
{
    QSharedPointer<QImage> resized = m_thumbnailResizeWatcher.result();
    // TODO
    // m_thumbnailResizeWatcher.setFuture(QFuture<QSharedPointer<QImage> >());
    m_thumbnailScaled.reset(new QImage(*resized));

    // Re-layout using scaled image
    m_renderer->setImage(m_thumbnailScaled.data(), false);
    prepareGeometryChange();
    m_renderer->layout();

    update(boundingRect());
    if (scene()) {
        emit requestLayout();
    }

    // We always consider it ready no matter whether thumbnail is null
    emit readyToShow();

    if (!m_isInsideViewportPreload) {
        qDebug() << "unload after image resized";
        resetThumbnail();
        unload();
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
    qDebug() << "set is inside viewport repload " << m_isInsideViewportPreload << "->"<< isInside;
    // if (m_isInsideViewportPreload && !isInside) {
    if (!isInside) {
        // Unload thumbnail
        // hide();
        resetThumbnail();
        unload();
    }

    if (!m_isInsideViewportPreload && isInside) {
    // if (isInside) {
        load();
        // show();
    }

    m_isInsideViewportPreload = isInside;
}

void GalleryItem::resetThumbnail()
{
    qDebug("reet htumb");
    m_thumbnail.reset();
    m_thumbnailScaled.reset();
    m_renderer->setImage(nullptr);
}
