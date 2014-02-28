#include <QGraphicsScene>
#include <QPainter>

#include "GlobalConfig.h"
#include "ImageGalleryItem.h"
#include "LooseImageRenderer.h"

static const int PADDING = 10;
static const int BORDER = 5;

ImageGalleryItem::ImageGalleryItem(Image *image,
                                   AbstractGalleryItemRenderer *renderer,
                                   QGraphicsItem *parent) :
    QGraphicsItem(parent) ,
    m_image(image) ,
    m_thumbnail(0) ,
    m_renderer(renderer)
{
    setFlag(QGraphicsItem::ItemIsSelectable);

    connect(m_image, SIGNAL(thumbnailLoaded()),
            this, SLOT(thumbnailLoaded()));
    m_image->loadThumbnail(true);
}

ImageGalleryItem::~ImageGalleryItem()
{
    delete m_renderer;
    if (m_thumbnail) {
        delete m_thumbnail;
    }
}

void ImageGalleryItem::setRenderer(AbstractGalleryItemRenderer *renderer)
{
    delete m_renderer;
    m_renderer = renderer;
    m_renderer->setImage(m_thumbnail);
    m_renderer->layout();
}

QRectF ImageGalleryItem::boundingRect() const
{
    return QRectF(QPointF(0, 0), GlobalConfig::instance()->galleryItemSize());
}

void ImageGalleryItem::thumbnailLoaded()
{
    const QImage &thumbnailImage = m_image->thumbnail();
    if (!thumbnailImage.isNull()) {
        m_thumbnail = new QImage(thumbnailImage);
        m_renderer->setImage(const_cast<QImage *>(m_thumbnail));
        m_renderer->layout();
        update(boundingRect());
    }
}

void ImageGalleryItem::paint(QPainter *painter,
                        const QStyleOptionGraphicsItem *,
                        QWidget *)
{
    painter->setRenderHints(
        QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    painter->setBrush(scene()->palette().foreground());
    painter->setBackground(scene()->palette().background());

    // Background
    if (isSelected()) {
        painter->drawRect(boundingRect());
    } else {
        painter->eraseRect(boundingRect());
    }

    m_renderer->paint(painter);
}
