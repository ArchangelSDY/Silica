#include <QDir>
#include <QFileInfo>
#include <QUrl>

#include "LocalImageSource.h"
#include "LocalImageSourceFactory.h"

LocalImageSourceFactory::LocalImageSourceFactory(ImageSourceManager *mgr) :
    ImageSourceFactory(mgr)
{
}

QString LocalImageSourceFactory::name() const
{
    return "Images";
}

QString LocalImageSourceFactory::fileNamePattern() const
{
    return "*.png *.jpg *.gif *.webp";
}

QString LocalImageSourceFactory::urlScheme() const
{
    return "file";
}

ImageSource *LocalImageSourceFactory::createSingle(const QUrl &url)
{
    if (url.scheme() == urlScheme() && isValidFileName(url.path())) {
        return new LocalImageSource(this, url.toLocalFile());
    } else {
        return 0;
    }
}

ImageSource *LocalImageSourceFactory::createSingle(const QString &path)
{
    if (isValidFileName(path)) {
        return new LocalImageSource(this, path);
    } else {
        return 0;
    }
}

QList<ImageSource *> LocalImageSourceFactory::createMultiple(const QUrl &url)
{
    return createMultiple(url.toLocalFile());
}

QList<ImageSource *> LocalImageSourceFactory::createMultiple(const QString &path)
{
    QList<ImageSource *> imageSources;
    QFileInfo file(path);

    if (!file.exists()) {
        return imageSources;
    }

    if (file.isDir()) {
        QDir dir(path);
        QStringList filters = fileNamePattern().split(" ");
        dir.setNameFilters(filters);

        foreach (const QFileInfo& fileInfo, dir.entryInfoList()) {
            imageSources << createSingle(fileInfo.absoluteFilePath());
        }
    } else {
        ImageSource *source = createSingle(path);
        if (source) {
            imageSources << createSingle(path);
        }
    }

    return imageSources;
}
