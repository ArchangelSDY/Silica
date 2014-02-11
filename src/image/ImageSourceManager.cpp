#include <QStringList>
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
