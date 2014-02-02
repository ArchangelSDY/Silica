#include <QGraphicsScene>

#include "GalleryItem.h"
#include "../GlobalConfig.h"

static int PADDING = 10;

GalleryItem::GalleryItem(Image *image, QGraphicsItem *parent) :
    QGraphicsPixmapItem(parent) ,
    m_image(image)
{
    if (m_image) {
        connect(m_image, SIGNAL(thumbnailLoaded()),
                this, SLOT(thumbnailLoaded()));
        m_image->loadThumbnail(true);
    }
}

void GalleryItem::thumbnailLoaded()
{
    if (!m_image) {
        return;
    }

    const QImage &thumbnailImage = m_image->thumbnail();
    if (!thumbnailImage.isNull()) {
        QPixmap thumbnail = QPixmap::fromImage(m_image->thumbnail());

        const QSize &itemSize = GlobalConfig::instance()->galleryItemSize();
        QSize imageSize(itemSize.width() - 2 * PADDING,
                        itemSize.height() - 2 * PADDING);

        QPixmap scaledThumbnail = thumbnail.scaled(imageSize,
            Qt::KeepAspectRatio, Qt::SmoothTransformation);

        int top = (itemSize.height() - scaledThumbnail.height()) / 2;
        int left = (itemSize.width() - scaledThumbnail.width()) / 2;

        setOffset(left, top);
        setPixmap(scaledThumbnail);
    }
}

void GalleryItem::paint(QPainter *painter,
                        const QStyleOptionGraphicsItem *,
                        QWidget *)
{
    // FIXME: Seems no top padding for first line item?
    painter->drawPixmap(offset(), pixmap());
}
