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
    m_thumbnail(0) ,
    m_thumbnailScaled(0) ,
    m_selectedAfterShownScheduled(false)
{
    connect(&m_thumbnailResizeWatcher, SIGNAL(finished()), this, SLOT(onThumbnailResized()));

    // Hide until thumbnail is ready.
    // It will show during next view layout after thumbnail is loaded
    //
    // This strategy is to hide the crazy rapid layout procedure when loading
    // remote gallery in waterfall mode.
    hide();
}

GalleryItem::~GalleryItem()
{
    if (m_renderer) {
        delete m_renderer;
    }
    if (m_thumbnail) {
        delete m_thumbnail;
    }
    if (m_thumbnailScaled) {
        delete m_thumbnailScaled;
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

    setThumbnail(m_thumbnail);
}

void GalleryItem::setRendererFactory(AbstractRendererFactory *factory)
{
    m_rendererFactory = factory;
    createRenderer();
}

static QSharedPointer<QImage> resizeThumbnailAtBackground(QSharedPointer<QImage> thumbnail, const QSize &size)
{
    return QSharedPointer<QImage>::create(thumbnail->scaled(size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
}

void GalleryItem::setThumbnail(QImage *thumbnail)
{
    if (m_thumbnail && m_thumbnail != thumbnail) {
        delete m_thumbnail;
    }
    m_thumbnail = thumbnail;

    // Some renderer depends on thumbnail aspect ratio to determine bounding rect.
    // So before making resized thumbnail image, we layout once using original image.
    m_renderer->setImage(m_thumbnail);
    m_renderer->layout();

    if (thumbnail) {
        QFuture<QSharedPointer<QImage> > resizeFuture = QtConcurrent::run(
            gResizeThumbnailThreads(), resizeThumbnailAtBackground,
            QSharedPointer<QImage>::create(*thumbnail), boundingRect().size().toSize());
        m_thumbnailResizeWatcher.setFuture(resizeFuture);
    }
}

void GalleryItem::onThumbnailResized()
{
    QSharedPointer<QImage> resized = m_thumbnailResizeWatcher.result();

    if (m_thumbnailScaled) {
        delete m_thumbnailScaled;
    }
    m_thumbnailScaled = new QImage(*resized);

    // Re-layout using scaled image
    m_renderer->setImage(m_thumbnailScaled);
    prepareGeometryChange();
    m_renderer->layout();

    update(boundingRect());
    if (scene()) {
        emit requestLayout();
    }

    // We always consider it ready no matter whether thumbnail is null
    emit readyToShow();
}

bool GalleryItem::isReadyToShow()
{
    return m_thumbnail != 0;
}

void GalleryItem::scheduleSelectedAfterShown()
{
    m_selectedAfterShownScheduled = true;
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

    // Render
    m_renderer->paint(painter);

    // Draw a mask if selected
    if (isSelected()) {
        painter->fillRect(boundingRect(), GalleryItem::SELECTED_COLOR);
    }
}

QVariant GalleryItem::itemChange(GraphicsItemChange change,
                                 const QVariant &value)
{
    if (change == ItemVisibleHasChanged) {
        if (value.toBool() && m_selectedAfterShownScheduled) {
            setSelected(true);
            m_selectedAfterShownScheduled = false;
        }
    }
    return QGraphicsItem::itemChange(change, value);
}
