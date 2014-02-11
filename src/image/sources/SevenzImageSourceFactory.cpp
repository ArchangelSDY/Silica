#include <QUrl>

#include "SevenzImageSource.h"
#include "SevenzImageSourceFactory.h"

QString SevenzImageSourceFactory::name() const
{
    return "7z";
}

QString SevenzImageSourceFactory::fileNamePattern() const
{
    return "*.7z";
}

QString SevenzImageSourceFactory::urlScheme() const
{
    return "sevenz";
}

ImageSource *SevenzImageSourceFactory::create(const QUrl &url)
{
    if (url.scheme() == urlScheme()) {
        QString imageName = url.fragment();

        QUrl sevenzUrl = url;
        sevenzUrl.setScheme("file");
        sevenzUrl.setFragment("");
        QString sevenzPath = sevenzUrl.toLocalFile();

        return new SevenzImageSource(sevenzPath, imageName);
    } else {
        return 0;
    }
}

ImageSource *SevenzImageSourceFactory::create(const QString &path)
{
    Q_UNUSED(path);
    Q_UNIMPLEMENTED();
    return 0;
}
