#include "ImageEffectDelegate.h"

#include "ImageDelegate.h"

namespace sapi {

ImageEffectDelegate::ImageEffectDelegate(IImageEffectPlugin *plugin) :
    m_plugin(plugin)
{
}

void ImageEffectDelegate::process(Image *image, QImage &frame)
{
    ImageDelegate imageDelegate(image);
    m_plugin->process(&imageDelegate, frame);
}

}