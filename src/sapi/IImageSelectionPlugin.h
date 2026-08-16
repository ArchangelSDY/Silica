#pragma once

#include <QImage>

#include "sapi/sapi_global.h"

namespace sapi {

class SAPI_EXPORT IImageSelectionPlugin
{
public:
    virtual ~IImageSelectionPlugin();

    // Invoked serially on a background thread. Implementations must not access UI objects directly.
    virtual void handleSelection(QImage image) = 0;
};

}

Q_DECLARE_INTERFACE(sapi::IImageSelectionPlugin, "com.archangelsdy.silica.plugins.imageselection")
