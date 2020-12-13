#include <QDir>
#include <QDirIterator>
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
    return "*.png *.jpg *.bmp *.gif *.webp";
}

QString LocalImageSourceFactory::urlScheme() const
{
    return "file";
}

ImageSource *LocalImageSourceFactory::createSingle(const QUrl &url)
{
    if (url.scheme() == urlScheme() && isValidFileName(url.path())) {
        return new LocalImageSource(this, findRealPath(url.toLocalFile()));
    } else {
        return 0;
    }
}

ImageSource *LocalImageSourceFactory::createSingle(const QString &path)
{
    if (isValidFileName(path)) {
        return new LocalImageSource(this, findRealPath(path));
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
        QStringList filters = fileNamePattern().split(" ");
        QDirIterator dirIter(path, filters, QDir::Files, QDirIterator::Subdirectories);

        while (dirIter.hasNext()) {
            ImageSource *source = createSingle(dirIter.next());
            if (source) {
                imageSources << source;
            }
        }
    } else {
        ImageSource *source = createSingle(path);
        if (source) {
            imageSources << source;
        }
    }

    return imageSources;
}
