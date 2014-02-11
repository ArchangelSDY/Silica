#include <QDir>
#include <QFileInfo>
#include <QUrl>

#include "LocalImageSource.h"
#include "LocalImageSourceFactory.h"

QString LocalImageSourceFactory::name() const
{
    return "Images";
}

QString LocalImageSourceFactory::fileNamePattern() const
{
    return "*.png *.jpg";
}

QString LocalImageSourceFactory::urlScheme() const
{
    return "file";
}

ImageSource *LocalImageSourceFactory::createSingle(const QUrl &url)
{
    if (url.scheme() == urlScheme()) {
        return new LocalImageSource(url.toLocalFile());
    } else {
        return 0;
    }
}

ImageSource *LocalImageSourceFactory::createSingle(const QString &path)
{
    return new LocalImageSource(path);
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
        imageSources << createSingle(path);
    }

    return imageSources;
}
