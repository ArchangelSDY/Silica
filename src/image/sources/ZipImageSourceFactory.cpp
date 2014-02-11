#include <QUrl>

#include "ZipImageSource.h"
#include "ZipImageSourceFactory.h"

QString ZipImageSourceFactory::name() const
{
    return "Zip";
}

QString ZipImageSourceFactory::fileNamePattern() const
{
    return "*.zip";
}

QString ZipImageSourceFactory::urlScheme() const
{
    return "zip";
}

ImageSource *ZipImageSourceFactory::create(const QUrl &url)
{
    if (url.scheme() == urlScheme()) {
        QString imageName = url.fragment();

        QUrl zipUrl = url;
        zipUrl.setScheme("file");
        zipUrl.setFragment("");
        QString zipPath = zipUrl.toLocalFile();

        return new ZipImageSource(zipPath, imageName);
    } else {
        return 0;
    }
}

ImageSource *ZipImageSourceFactory::create(const QString &path)
{
    Q_UNUSED(path);
    Q_UNIMPLEMENTED();
    return 0;
}
