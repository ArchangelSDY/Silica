#include "ImageEffectManager.h"

#include "sapi/IImageEffectPlugin.h"
#include "sapi/ImageEffectDelegate.h"
#include "sapi/PluginLoader.h"

ImageEffectManager::ImageEffectManager()
{
    sapi::PluginLoadCallback<sapi::IImageEffectPlugin> callback = [this](sapi::IImageEffectPlugin *plugin, const QJsonObject &meta) {
        sapi::ImageEffectDelegate *effect = new sapi::ImageEffectDelegate(plugin);
        this->m_effects << effect;
    };

    sapi::loadPlugins("imageeffects", callback);
}

ImageEffectManager::~ImageEffectManager()
{
    for (ImageEffect *effect : m_effects) {
        delete effect;
    }
}

QImage ImageEffectManager::process(QSharedPointer<Image> image, const QImage &frame)
{
    QImage ret = frame;

    for (ImageEffect *effect : m_effects) {
        effect->process(image, ret);
    }

    return ret;
}