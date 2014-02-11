#include <QUrl>
#include <Qt7zPackage.h>

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

ImageSource *SevenzImageSourceFactory::createSingle(const QUrl &url)
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

ImageSource *SevenzImageSourceFactory::createSingle(const QString &path)
{
    Q_UNUSED(path);
    Q_UNIMPLEMENTED();
    return 0;
}

QList<ImageSource *> SevenzImageSourceFactory::createMultiple(const QUrl &url)
{
    QList<ImageSource *> imageSources;

    if (url.hasFragment()) {
        imageSources << createSingle(url);
    } else {
        // Add all files in the 7z package
        // TODO: Should skip non image files
        QUrl fileUrl = url;
        fileUrl.setScheme("file");
        QString packagePath = fileUrl.toLocalFile();

        Qt7zPackage pkg(packagePath);
        bool success = pkg.open();

        if (success) {
            QStringList fileNameList = pkg.getFileNameList();

            foreach(const QString &name, fileNameList) {
                QUrl imageUrl = url;
                imageUrl.setFragment(name);

                imageSources << createSingle(imageUrl);
            }

            pkg.close();
        }
    }

    return imageSources;
}

QList<ImageSource *> SevenzImageSourceFactory::createMultiple(const QString &path)
{
    Q_UNUSED(path);
    Q_UNIMPLEMENTED();
    return QList<ImageSource *>();
}
