#pragma once

#include <QImage>

#include "sapi_global.h"

namespace sapi {

class ImageResource;

class SAPI_EXPORT IImageEffectPlugin
{
public:
    virtual ~IImageEffectPlugin();

    virtual void process(ImageResource *image, QImage &frame) = 0;

};

}

Q_DECLARE_INTERFACE(sapi::IImageEffectPlugin, "com.archangelsdy.silica.plugins.imageeffect")