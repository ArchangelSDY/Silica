#include "ImageThumbnailBlockLoader.h"

ImageThumbnailBlockLoader::ImageThumbnailBlockLoader(ImagePtr image) :
    m_image(image) ,
    m_isFinished(false)
{
    QObject::connect(image.data(), &Image::thumbnailLoaded, this, &ImageThumbnailBlockLoader::thumbnailLoaded);
    QObject::connect(image.data(), &Image::thumbnailLoadFailed, &m_loop, &QEventLoop::quit);
}

void ImageThumbnailBlockLoader::loadAndWait()
{
    m_image->loadThumbnail();

    // Do not enter loop if loading is sync and already finished
    if (m_isFinished) {
        return;
    }

    // TODO: Add timeout
    m_loop.exec();
}

QSharedPointer<QImage> ImageThumbnailBlockLoader::thumbnail() const
{
    return m_thumbnail;
}

void ImageThumbnailBlockLoader::thumbnailLoaded(QSharedPointer<QImage> thumbnail)
{
    m_isFinished = true;
    m_loop.quit();
    m_thumbnail = thumbnail;
    m_image->unloadThumbnail();
}
