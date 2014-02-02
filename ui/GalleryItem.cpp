#include <QGraphicsScene>

#include "GalleryItem.h"
#include "../GlobalConfig.h"

GalleryItem::GalleryItem(Image *image, QGraphicsItem *parent) :
    QGraphicsPixmapItem(parent) ,
    m_image(image)
{
    // FIXME: Make thumbnail immediately when not available
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
        QPixmap scaledThumbnail = thumbnail.scaled(
            GlobalConfig::instance()->galleryItemSize(),
            Qt::KeepAspectRatio, Qt::SmoothTransformation);

        setPixmap(scaledThumbnail);
    }
}
