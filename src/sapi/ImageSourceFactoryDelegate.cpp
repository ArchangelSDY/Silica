#include "ImageSourceFactoryDelegate.h"

#include "image/ImageSource.h"
#include "IImageSourceFactory.h"
#include "IImageSourcePlugin.h"
#include "ImageSourceDelegate.h"

namespace sapi {

ImageSourceFactoryDelegate::ImageSourceFactoryDelegate(IImageSourcePlugin *plugin,
                                                       ImageSourceManager *mgr,
                                                       IImageSourceFactory *factory) :
    ImageSourceFactory(mgr) ,
    m_plugin(plugin) ,
    m_factory(factory)
{
}

ImageSourceFactoryDelegate::~ImageSourceFactoryDelegate()
{
    m_plugin->destroyFactory(m_factory);
}

QString ImageSourceFactoryDelegate::name() const
{
    return m_factory->name();
}

QStringList ImageSourceFactoryDelegate::fileNameSuffixes() const
{
    return m_factory->fileNameSuffixes();
}

QString ImageSourceFactoryDelegate::urlScheme() const
{
    return m_factory->urlScheme();
}

ImageSource *ImageSourceFactoryDelegate::createSingle(const QUrl &url)
{
    IImageSource *source = m_factory->createSingle(url);
    if (source) {
        return new ImageSourceDelegate(this, source);
    } else {
        return nullptr;
    }
}

ImageSource *ImageSourceFactoryDelegate::createSingle(const QString &path)
{
    IImageSource *source = m_factory->createSingle(path);
    if (source) {
        return new ImageSourceDelegate(this, source);
    } else {
        return nullptr;
    }
}

QList<ImageSource *> ImageSourceFactoryDelegate::createMultiple(const QUrl &url)
{
    QList<ImageSource *> sources;

    QList<IImageSource *> pluginSources = m_factory->createMultiple(url);
    for (IImageSource *pluginSource : pluginSources) {
        sources << new ImageSourceDelegate(this, pluginSource);
    }

    return sources;
}

QList<ImageSource *> ImageSourceFactoryDelegate::createMultiple(const QString &path)
{
    QList<ImageSource *> sources;

    QList<IImageSource *> pluginSources = m_factory->createMultiple(path);
    for (IImageSource *pluginSource : pluginSources) {
        sources << new ImageSourceDelegate(this, pluginSource);
    }

    return sources;
}

}