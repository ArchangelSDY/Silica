#include "ZipImageSourceFactory.h"

#include <QFileInfo>
#include <QImageReader>
#include <QUrl>

#include "deps/quazip/quazip/quazip.h"
#include "ZipImageSource.h"

ZipImageSourceFactory::ZipImageSourceFactory(ImageSourceManager *mgr) :
    ImageSourceFactory(mgr)
{
}

QString ZipImageSourceFactory::name() const
{
    return "Zip";
}

QStringList ZipImageSourceFactory::fileNameSuffixes() const
{
    return { "zip" };
}

QString ZipImageSourceFactory::urlScheme() const
{
    return "zip";
}

ImageSource *ZipImageSourceFactory::createSingle(const QUrl &url)
{
    if (url.scheme() == urlScheme()) {
        QString imageName = url.fragment(QUrl::FullyDecoded);
        QFileInfo imageFile(imageName);
        if (QImageReader::supportedImageFormats().contains(
                imageFile.suffix().toUtf8().toLower())) {
            QUrl zipUrl = url;
            zipUrl.setScheme("file");
            zipUrl.setFragment("");
            QString zipPath = zipUrl.toLocalFile();
            QString realZipPath = findRealPath(zipPath);

            return new ZipImageSource(this, realZipPath, imageName);
        } else {
            // Unsupported image format
            return 0;
        }
    } else {
        return 0;
    }
}

ImageSource *ZipImageSourceFactory::createSingle(const QString &packagePath)
{
    if (!isValidFileName(packagePath)) {
        return nullptr;
    }

    QString realPackagePath = findRealPath(packagePath);

    QuaZip zip(realPackagePath);
    bool success = zip.open(QuaZip::mdUnzip);
    if (!success) {
        return nullptr;
    }

    QStringList fileNameList = zip.getFileNameList();
    if (fileNameList.isEmpty()) {
        return nullptr;
    }

    foreach(const QString &imageName, fileNameList) {
        QFileInfo nameInfo(imageName);
        if (QImageReader::supportedImageFormats().contains(nameInfo.suffix().toUtf8().toLower())) {
            return new ZipImageSource(this, packagePath, imageName);
        }
    }

    return nullptr;
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
        QString realPackagePath = findRealPath(packagePath);

        QuaZip zip(realPackagePath);
        bool success = zip.open(QuaZip::mdUnzip);

        if (success) {
            QStringList fileNameList = zip.getFileNameList();

            foreach(const QString &name, fileNameList) {
                QFileInfo nameInfo(name);
                if (QImageReader::supportedImageFormats().contains(nameInfo.suffix().toUtf8().toLower())) {
                    imageSources << new ZipImageSource(this, realPackagePath, name);
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
