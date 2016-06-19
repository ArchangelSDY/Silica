#pragma once

#include <QObject>

#include "sapi/sapi_global.h"
#include "sapi/IImageSourceFactory.h"

namespace sapi {

class SAPI_EXPORT IImageSourcePlugin
{
public:
    virtual ~IImageSourcePlugin();

    virtual IImageSourceFactory *createFactory() = 0;
    virtual void destroyFactory(IImageSourceFactory *factory) = 0;
};

}

Q_DECLARE_INTERFACE(sapi::IImageSourcePlugin, "com.archangelsdy.silica.plugins.imagesource")