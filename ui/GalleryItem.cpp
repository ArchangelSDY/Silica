#include <QGraphicsScene>

#include "GalleryItem.h"
#include "../GlobalConfig.h"

static int PADDING = 10;

GalleryItem::GalleryItem(Image *image, QGraphicsItem *parent) :
    QGraphicsItem(parent) ,
    m_image(image)
{
    if (m_image) {
        connect(m_image, SIGNAL(thumbnailLoaded()),
                this, SLOT(thumbnailLoaded()));
        m_image->loadThumbnail(true);
    }
}

QRectF GalleryItem::boundingRect() const
{
    return QRectF(QPointF(0, 0), GlobalConfig::instance()->galleryItemSize());
}

void GalleryItem::thumbnailLoaded()
{
    if (!m_image) {
        return;
    }

    const QImage &thumbnailImage = m_image->thumbnail();
    if (!thumbnailImage.isNull()) {
        // Calculate image size and position
        const QSize &itemSize = GlobalConfig::instance()->galleryItemSize();
        m_imageSize.setWidth(itemSize.width() - 2 * PADDING);
        m_imageSize.setHeight(itemSize.height() - 2 * PADDING);

        m_imageSize = m_image->thumbnail().size().scaled(
            m_imageSize, Qt::KeepAspectRatio);

        m_imagePos.setX((itemSize.width() - m_imageSize.width()) / 2);
        m_imagePos.setY((itemSize.height() - m_imageSize.height()) / 2);
    }
}

void GalleryItem::selectionChange(int index)
{
    index = scene()->items().length() - index - 1;
    if (scene()->items().at(index) == this) {

    }
}

void GalleryItem::paint(QPainter *painter,
                        const QStyleOptionGraphicsItem *,
                        QWidget *)
{
    painter->drawImage(QRect(m_imagePos, m_imageSize), m_image->thumbnail());
}
