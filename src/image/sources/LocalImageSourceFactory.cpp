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

ImageSource *LocalImageSourceFactory::create(const QUrl &url)
{
    if (url.scheme() == urlScheme()) {
        return new LocalImageSource(url.toLocalFile());
    } else {
        return 0;
    }
}

ImageSource *LocalImageSourceFactory::create(const QString &path)
{
    return new LocalImageSource(path);
}
