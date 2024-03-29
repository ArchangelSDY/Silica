#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QSet>
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

QStringList LocalImageSourceFactory::fileNameSuffixes() const
{
    return { "png", "jpg", "bmp", "gif", "webp" };
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
        auto suffixes = fileNameSuffixes();
        QSet<QString> validSuffixes(suffixes.begin(), suffixes.end());
        QDirIterator dirIter(path, QDir::Files, QDirIterator::Subdirectories);
        while (dirIter.hasNext()) {
            QString path = dirIter.next();
            if (!validSuffixes.contains(dirIter.fileInfo().suffix())) {
                continue;
            }
            ImageSource *source = createSingle(path);
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
