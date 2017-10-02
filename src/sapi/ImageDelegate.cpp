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
    return image->extraInfo();
}

bool ImageDelegate::isAnimation() const
{
    return m_image->isAnimation();
}

int ImageDelegate::frameCount() const
{
    return m_image->frameCount();
}

QList<int> ImageDelegate::durations() const
{
    return m_image->durations();
}

QList<const QImage *> ImageDelegate::frames() const
{
   QList<const QImage *> frames;
   foreach (QImage *image, m_image->frames()) {
       frames << image;
   }
   return frames;
}

QImage ImageDelegate::thumbnail() const
{
    Image *image = const_cast<Image *>(m_image);
    return *image->thumbnail();
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

