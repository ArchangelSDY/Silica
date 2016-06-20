#include "CGColleImageSourceFactory.h"

#include <QRegExp>
#include <QUrl>

#include "CGColleImageSource.h"
#include "CGColleReader.h"

CGColleImageSourceFactory::CGColleImageSourceFactory(ImageSourceManager *mgr) :
    ImageSourceFactory(mgr)
{
}

QString CGColleImageSourceFactory::name() const
{
    return "CGC";
}

QString CGColleImageSourceFactory::fileNamePattern() const
{
    return "*.cgc";
}

QString CGColleImageSourceFactory::urlScheme() const
{
    return "cgc";
}

ImageSource *CGColleImageSourceFactory::createSingle(const QUrl &url)
{
    if (url.scheme() == urlScheme()) {
        QString imageName = url.fragment();
        QUrl packageUrl = url;
        packageUrl.setScheme("file");
        packageUrl.setFragment("");
        QString packagePath = packageUrl.toLocalFile();

        return new CGColleImageSource(this, packagePath, imageName);
    } else {
        return 0;
    }
}

ImageSource *CGColleImageSourceFactory::createSingle(const QString &)
{
    return 0;
}

QList<ImageSource *> CGColleImageSourceFactory::createMultiple(const QUrl &url)
{
    QList<ImageSource *> imageSources;

    if (!isValidFileName(url.path())) {
        return imageSources;
    }

    if (url.hasFragment()) {
        ImageSource *source = createSingle(url);
        if (source) {
            imageSources << source;
        }
    } else {
        // Add all files in the zip
        QUrl fileUrl = url;
        fileUrl.setScheme("file");
        QString packagePath = fileUrl.toLocalFile();

        CGColleReader package(packagePath);
        bool success = package.open();

        if (success) {
            QStringList imageNames = package.imageNames();

            foreach(const QString &name, imageNames) {
                QUrl imageUrl = url;
                imageUrl.setFragment(name);

                ImageSource *source = createSingle(imageUrl);
                if (source) {
                    imageSources << source;
                }
            }

            package.close();
        }
    }

    return imageSources;
}

QList<ImageSource *> CGColleImageSourceFactory::createMultiple(const QString &path)
{
    QUrl url = QUrl::fromLocalFile(path);
    url.setScheme(urlScheme());
    return createMultiple(url);
}
