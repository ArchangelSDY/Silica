#include "ImageGalleryItemModel.h"

ImageGalleryItemModel::ImageGalleryItemModel(Image *image, QObject *parent) :
    AbstractGalleryItemModel(parent) ,
    m_image(image)
{
}

void ImageGalleryItemModel::loadThumbnail()
{
    Q_ASSERT(m_image);
    connect(m_image, SIGNAL(thumbnailLoaded()),
            this, SIGNAL(thumbnailLoaded()));
    m_image->loadThumbnail(true);
}

QImage ImageGalleryItemModel::thumbnail() const
{
    Q_ASSERT(m_image);
    return m_image->thumbnail();
}
