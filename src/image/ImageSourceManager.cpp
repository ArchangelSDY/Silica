#include <QUrl>

#include "ImageSourceManager.h"
#include "LocalImageSourceFactory.h"
#include "ZipImageSourceFactory.h"

ImageSourceManager *ImageSourceManager::m_instance = 0;

ImageSourceManager::ImageSourceManager(QObject *parent) :
    QObject(parent)
{
    registerFactory(new LocalImageSourceFactory());
    registerFactory(new ZipImageSourceFactory());
}

ImageSourceManager *ImageSourceManager::instance()
{
    if (!m_instance) {
        m_instance = new ImageSourceManager();
    }

    return m_instance;
}

ImageSourceManager::~ImageSourceManager()
{
    for (QHash<QString, ImageSourceFactory *>::iterator it = m_factories.begin();
         it != m_factories.end(); ++it) {
        delete it.value();
    }
}

ImageSource *ImageSourceManager::create(const QUrl &url)
{
    if (m_factories.contains(url.scheme())) {
        ImageSourceFactory *factory = m_factories.value(url.scheme());
        return factory->create(url);
    } else {
        return 0;
    }
}

ImageSource *ImageSourceManager::create(const QString &path)
{
    for (QHash<QString, ImageSourceFactory *>::iterator it = m_factories.begin();
         it != m_factories.end(); ++it) {
        ImageSourceFactory *factory = it.value();
        ImageSource *imageSource = factory->create(path);

        if (imageSource) {
            return imageSource;
        }
    }

    return 0;
}

void ImageSourceManager::registerFactory(ImageSourceFactory *factory)
{
    if (factory) {
        m_factories.insert(factory->urlScheme(), factory);
    }
}
