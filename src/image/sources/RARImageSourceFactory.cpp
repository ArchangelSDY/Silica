#include <QFileInfo>
#include <QImageReader>
#include <QString>

#include "deps/QtRAR/src/qtrar.h"

#include "RARImageSource.h"
#include "RARImageSourceFactory.h"

RARImageSourceFactory::RARImageSourceFactory(ImageSourceManager *mgr) :
    ImageSourceFactory(mgr)
{
}

QString RARImageSourceFactory::name() const
{
    return "RAR";
}

QString RARImageSourceFactory::fileNamePattern() const
{
    return "*.rar";
}

QString RARImageSourceFactory::urlScheme() const
{
    return "rar";
}

ImageSource *RARImageSourceFactory::createSingle(const QUrl &url)
{
    return createSingle(url, QByteArray());
}

ImageSource *RARImageSourceFactory::createSingle(const QUrl &url,
                                                 const QByteArray &password)
{
    if (url.scheme() == urlScheme()) {
        QString imageName = url.fragment();
        QFileInfo imageFile(imageName);
        if (QImageReader::supportedImageFormats().contains(
                imageFile.suffix().toUtf8())) {
            QUrl rarUrl = url;
            rarUrl.setScheme("file");
            rarUrl.setFragment("");
            QString rarPath = rarUrl.toLocalFile();

            return new RARImageSource(this, rarPath, imageName, password);
        } else {
            // Unsupported image format
            return 0;
        }
    } else {
        return 0;
    }
}

ImageSource *RARImageSourceFactory::createSingle(const QString &)
{
    return 0;
}

QList<ImageSource *> RARImageSourceFactory::createMultiple(const QUrl &url)
{
    QList<ImageSource *> imageSources;

    if (!isValidFileName(url.path())) {
        return imageSources;
    }

    if (url.hasFragment()) {
        imageSources << createSingle(url);
    } else {
        // Add all files in the zip
        QUrl fileUrl = url;
        fileUrl.setScheme("file");
        QString packagePath = fileUrl.toLocalFile();

        QtRAR rar(packagePath);
        bool success = rar.open(QtRAR::OpenModeList);

        if (success) {
            QByteArray password;
            if (rar.isHeadersEncrypted()) {
                rar.close();

                if (!requestPassword(password)) {
                    return imageSources;
                }

                success = rar.open(QtRAR::OpenModeList, password);
                if (!success) {
                    return imageSources;
                }
            }

            QStringList fileNameList = rar.fileNameList();

            foreach(const QString &name, fileNameList) {
                QUrl imageUrl = url;
                imageUrl.setFragment(name);

                ImageSource *source = createSingle(imageUrl, password);
                if (source) {
                    imageSources << source;
                }
            }

            rar.close();
        }
    }

    return imageSources;
}

QList<ImageSource *> RARImageSourceFactory::createMultiple(const QString &path)
{
    QUrl url = QUrl::fromLocalFile(path);
    url.setScheme(urlScheme());
    return createMultiple(url);
}
