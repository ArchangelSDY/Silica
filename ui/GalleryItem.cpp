#include <QGraphicsScene>

#include "GalleryItem.h"
#include "../GlobalConfig.h"

static const int PADDING = 10;
static const int BORDER = 5;

GalleryItem::GalleryItem(Image *image, QGraphicsItem *parent) :
    QGraphicsItem(parent) ,
    m_image(image) ,
    m_selected(false)
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

        m_borderRect.setX(m_imagePos.x() - BORDER);
        m_borderRect.setY(m_imagePos.y() - BORDER);
        m_borderRect.setWidth(m_imageSize.width() + 2 * BORDER);
        m_borderRect.setHeight(m_imageSize.height() + 2 * BORDER);
    }
}

void GalleryItem::selectionChange(int index)
{
    index = scene()->items().length() - index - 1;
    if (scene()->items().at(index) == this) {
        m_selected = true;
    } else {
        m_selected = false;
    }
    update(boundingRect());
}

void GalleryItem::paint(QPainter *painter,
                        const QStyleOptionGraphicsItem *,
                        QWidget *)
{
    // Background
    if (m_selected) {
        painter->fillRect(boundingRect(), Qt::darkCyan);
    } else {
        painter->fillRect(boundingRect(), Qt::gray);
    }

    // Border
     painter->setBrush(Qt::white);
     painter->setPen(Qt::white);
     painter->drawRoundedRect(m_borderRect, BORDER, BORDER);

    // Image
    painter->drawImage(QRect(m_imagePos, m_imageSize), m_image->thumbnail());
}
