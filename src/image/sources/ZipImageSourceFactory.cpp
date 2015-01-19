#include <QFileInfo>
#include <QImageReader>
#include <QUrl>
#include <quazip.h>

#include "ZipImageSource.h"
#include "ZipImageSourceFactory.h"

ZipImageSourceFactory::ZipImageSourceFactory(ImageSourceManager *mgr) :
    ImageSourceFactory(mgr)
{
}

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

ImageSource *ZipImageSourceFactory::createSingle(const QUrl &url)
{
    if (url.scheme() == urlScheme()) {
        QString imageName = url.fragment();
        QFileInfo imageFile(imageName);
        if (QImageReader::supportedImageFormats().contains(
                imageFile.suffix().toUtf8())) {
            QUrl zipUrl = url;
            zipUrl.setScheme("file");
            zipUrl.setFragment("");
            QString zipPath = zipUrl.toLocalFile();

            return new ZipImageSource(this, zipPath, imageName);
        } else {
            // Unsupported image format
            return 0;
        }
    } else {
        return 0;
    }
}

ImageSource *ZipImageSourceFactory::createSingle(const QString &)
{
    return 0;
}

QList<ImageSource *> ZipImageSourceFactory::createMultiple(const QUrl &url)
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

        QuaZip zip(packagePath);
        bool success = zip.open(QuaZip::mdUnzip);

        if (success) {
            QStringList fileNameList = zip.getFileNameList();

            foreach(const QString &name, fileNameList) {
                QUrl imageUrl = url;
                imageUrl.setFragment(name);

                ImageSource *source = createSingle(imageUrl);
                if (source) {
                    imageSources << source;
                }
            }

            zip.close();
        }
    }

    return imageSources;
}

QList<ImageSource *> ZipImageSourceFactory::createMultiple(const QString &path)
{
    QUrl url = QUrl::fromLocalFile(path);
    url.setScheme(urlScheme());
    return createMultiple(url);
}
