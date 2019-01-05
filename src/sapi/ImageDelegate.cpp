#include "ImageDelegate.h"

#include "image/Image.h"
#include "image/ImageSource.h"
#include "image/ImageSourceManager.h"

namespace sapi {

ImageDelegate::ImageDelegate(const Image *image) :
    m_image(image)
{
}

QString ImageDelegate::name() const
{
    return m_image->name();
}

QSize ImageDelegate::size() const
{
    return m_image->size();
}

QString ImageDelegate::hash() const
{
    return m_image->source()->hashStr();
}

QUrl ImageDelegate::url() const
{
    return m_image->source()->url();
}

const QVariantHash &ImageDelegate::extraInfo() const
{
    Image *image = const_cast<Image *>(m_image);
    return image->metadata();
}

bool ImageDelegate::isAnimation() const
{
    QSharedPointer<ImageData> imageData = m_image->data().toStrongRef();
    if (!imageData) {
        return false;
    }

    return imageData->isAnimation();
}

int ImageDelegate::frameCount() const
{
    QSharedPointer<ImageData> imageData = m_image->data().toStrongRef();
    if (!imageData) {
        return 0;
    }

    return imageData->frames.count();
}

QList<int> ImageDelegate::durations() const
{
    QSharedPointer<ImageData> imageData = m_image->data().toStrongRef();
    if (!imageData) {
        return QList<int>();
    }

    return imageData->durations;
}

QList<const QImage *> ImageDelegate::frames() const
{
    QList<const QImage *> frames;
    
    QSharedPointer<ImageData> imageData = m_image->data().toStrongRef();
    if (!imageData) {
        return frames;
    }

    foreach (const QImage &image, imageData->frames) {
        frames << &image;
    }

    return frames;
}

QImage ImageDelegate::thumbnail() const
{
    // TODO: Deprecate this in favor of async version
    Image *image = const_cast<Image *>(m_image);
    QSharedPointer<QImage> thumbnail = image->loadThumbnailSync();
    return *thumbnail;
}

QByteArray ImageDelegate::readRaw() const
{
    ImageSource *source = ImageSourceManager::instance()->createSingle(
        m_image->source()->url());

    if (!source->open()) {
        return QByteArray();
    }

    QByteArray raw = source->device()->readAll();
    source->close();

    return raw;
}

}

