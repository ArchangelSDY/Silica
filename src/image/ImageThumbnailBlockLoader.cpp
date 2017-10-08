#include "ImageThumbnailBlockLoader.h"

ImageThumbnailBlockLoader::ImageThumbnailBlockLoader(ImagePtr image) :
    m_image(image)
{
    QObject::connect(image.data(), &Image::thumbnailLoaded, this, &ImageThumbnailBlockLoader::thumbnailLoaded);
    QObject::connect(image.data(), &Image::thumbnailLoadFailed, &m_loop, &QEventLoop::quit);
}

void ImageThumbnailBlockLoader::loadAndWait()
{
    // TODO: Add timeout
    m_image->loadThumbnail();
    m_loop.exec();
}

QSharedPointer<QImage> ImageThumbnailBlockLoader::thumbnail() const
{
    return m_thumbnail;
}

void ImageThumbnailBlockLoader::thumbnailLoaded(QSharedPointer<QImage> thumbnail)
{
    m_loop.quit();
    m_thumbnail = thumbnail;
    m_image->unloadThumbnail();
}
