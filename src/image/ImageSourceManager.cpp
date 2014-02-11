#include <QStringList>
#include <QUrl>

#include "ImageSourceManager.h"
#include "LocalImageSourceFactory.h"
#include "SevenzImageSourceFactory.h"
#include "ZipImageSourceFactory.h"

ImageSourceManager *ImageSourceManager::m_instance = 0;

ImageSourceManager::ImageSourceManager(QObject *parent) :
    QObject(parent)
{
    registerFactory(new LocalImageSourceFactory());
    registerFactory(new ZipImageSourceFactory());
    registerFactory(new SevenzImageSourceFactory());
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

ImageSource *ImageSourceManager::createSingle(const QUrl &url)
{
    if (m_factories.contains(url.scheme())) {
        ImageSourceFactory *factory = m_factories.value(url.scheme());
        return factory->createSingle(url);
    } else {
        return 0;
    }
}

ImageSource *ImageSourceManager::createSingle(const QString &path)
{
    for (QHash<QString, ImageSourceFactory *>::iterator it = m_factories.begin();
         it != m_factories.end(); ++it) {
        ImageSourceFactory *factory = it.value();
        ImageSource *imageSource = factory->createSingle(path);

        if (imageSource) {
            return imageSource;
        }
    }

    return 0;
}

QList<ImageSource *> ImageSourceManager::createMultiple(const QUrl &url)
{
    if (m_factories.contains(url.scheme())) {
        ImageSourceFactory *factory = m_factories.value(url.scheme());
        return factory->createMultiple(url);
    } else {
        return QList<ImageSource *>();
    }
}

QList<ImageSource *> ImageSourceManager::createMultiple(const QString &path)
{
    for (QHash<QString, ImageSourceFactory *>::iterator it = m_factories.begin();
         it != m_factories.end(); ++it) {
        ImageSourceFactory *factory = it.value();
        QList<ImageSource *> imageSources = factory->createMultiple(path);

        if (imageSources.count() > 0) {
            return imageSources;
        }
    }

    return QList<ImageSource *>();
}

QStringList ImageSourceManager::urlPatterns() const
{
    return QStringList(m_factories.keys());
}

QString ImageSourceManager::fileDialogFilters() const
{
    QStringList parts;
    QStringList fileNamePatterns;
    for (QHash<QString, ImageSourceFactory *>::const_iterator it = m_factories.begin();
         it != m_factories.end(); ++it) {
        ImageSourceFactory *factory = it.value();
        QString filterPart = QString("%1 (%2)").arg(factory->name(),
                                                    factory->fileNamePattern());
        parts << filterPart;

        fileNamePatterns << factory->fileNamePattern();
    }

    QString partForAll = QString("All (%1)").arg(fileNamePatterns.join(" "));
    parts.prepend(partForAll);

    return parts.join(";;");
}

void ImageSourceManager::registerFactory(ImageSourceFactory *factory)
{
    if (factory) {
        m_factories.insert(factory->urlScheme(), factory);
    }
}
