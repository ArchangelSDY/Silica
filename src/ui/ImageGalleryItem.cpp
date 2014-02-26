#include <QPainter>

#include "GlobalConfig.h"
#include "ImageGalleryItem.h"

static const int PADDING = 10;
static const int BORDER = 5;

ImageGalleryItem::ImageGalleryItem(Image *image,
                                   QGraphicsItem *parent) :
    QGraphicsItem(parent) ,
    m_image(image)
{
    setFlag(QGraphicsItem::ItemIsSelectable);

    connect(m_image, SIGNAL(thumbnailLoaded()),
            this, SLOT(thumbnailLoaded()));
    m_image->loadThumbnail(true);
}

QRectF ImageGalleryItem::boundingRect() const
{
    return QRectF(QPointF(0, 0), GlobalConfig::instance()->galleryItemSize());
}

void ImageGalleryItem::thumbnailLoaded()
{
    const QImage &thumbnailImage = m_image->thumbnail();
    if (!thumbnailImage.isNull()) {
        // Calculate image size and position
        const QSize &itemSize = GlobalConfig::instance()->galleryItemSize();
        m_thumbnailSize.setWidth(itemSize.width() - 2 * PADDING);
        m_thumbnailSize.setHeight(itemSize.height() - 2 * PADDING);

        m_thumbnailSize = thumbnailImage.size().scaled(
            m_thumbnailSize, Qt::KeepAspectRatio);

        m_thumbnailPos.setX((itemSize.width() - m_thumbnailSize.width()) / 2);
        m_thumbnailPos.setY((itemSize.height() - m_thumbnailSize.height()) / 2);

        m_borderRect.setX(m_thumbnailPos.x() - BORDER);
        m_borderRect.setY(m_thumbnailPos.y() - BORDER);
        m_borderRect.setWidth(m_thumbnailSize.width() + 2 * BORDER);
        m_borderRect.setHeight(m_thumbnailSize.height() + 2 * BORDER);

        update(boundingRect());
    }
}

void ImageGalleryItem::paint(QPainter *painter,
                        const QStyleOptionGraphicsItem *,
                        QWidget *)
{
    painter->setRenderHints(
        QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    // Background
    if (isSelected()) {
        painter->fillRect(boundingRect(), Qt::darkCyan);
    } else {
        painter->fillRect(boundingRect(), Qt::gray);
    }

    // Border
    painter->setBrush(Qt::white);
    painter->setPen(Qt::white);
    painter->drawRoundedRect(m_borderRect, BORDER, BORDER);

    // Image
    painter->drawImage(QRect(m_thumbnailPos, m_thumbnailSize),
                       m_image->thumbnail());
}
