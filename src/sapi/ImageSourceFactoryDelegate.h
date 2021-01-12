#pragma once

#include "image/ImageSourceFactory.h"

namespace sapi {

class IImageSourceFactory;
class IImageSourcePlugin;

class ImageSourceFactoryDelegate : public ImageSourceFactory
{
public:
    explicit ImageSourceFactoryDelegate(IImageSourcePlugin *plugin, ImageSourceManager *mgr, IImageSourceFactory *factory);
    virtual ~ImageSourceFactoryDelegate();

    virtual QString name() const;
    virtual QStringList fileNameSuffixes() const;
    virtual QString urlScheme() const;
    virtual ImageSource *createSingle(const QUrl &url);
    virtual ImageSource *createSingle(const QString &path);
    virtual QList<ImageSource *> createMultiple(const QUrl &url);
    virtual QList<ImageSource *> createMultiple(const QString &path);

private:
    IImageSourcePlugin *m_plugin;
    IImageSourceFactory *m_factory;
};

}