#pragma once

#include <QScopedPointer>

#include "../image/effects/ImageEffect.h"
#include "IImageEffectPlugin.h"

namespace sapi {

class ImageEffectDelegate : public ImageEffect
{
public:
    ImageEffectDelegate(IImageEffectPlugin *plugin);

    virtual void process(Image *image, QImage &frame) override;

private:
    QScopedPointer<IImageEffectPlugin> m_plugin;
};

}