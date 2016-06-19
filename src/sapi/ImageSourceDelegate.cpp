#include "ImageSourceDelegate.h"

#include "IImageSource.h"
#include "IImageSourcePlugin.h"

namespace sapi {

ImageSourceDelegate::ImageSourceDelegate(ImageSourceFactory *factory,
                                         IImageSource *source) :
    ImageSource(factory) ,
    m_source(source)
{
    m_name = source->name();
    m_hash = source->hash();
    m_url = source->url();
}

ImageSourceDelegate::~ImageSourceDelegate()
{
}

bool ImageSourceDelegate::open()
{
    if (!m_source->open()) {
        return false;
    }

    m_device.reset(m_source->device());
    return true;
}

void ImageSourceDelegate::close()
{
    m_device.take();
    m_source->close();
}

bool ImageSourceDelegate::exists()
{
    return m_source->exists();
}

bool ImageSourceDelegate::copy(const QString &destPath)
{
    // Not implemented
    return false;
}


}