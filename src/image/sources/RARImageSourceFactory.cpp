#include <QFileInfo>
#include <QImageReader>

#include "deps/QtRAR/src/qtrar.h"

#include "RARImageSource.h"
#include "RARImageSourceFactory.h"

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
    if (url.scheme() == urlScheme()) {
        QString imageName = url.fragment();
        QFileInfo imageFile(imageName);
        if (QImageReader::supportedImageFormats().contains(
                imageFile.suffix().toUtf8())) {
            QUrl rarUrl = url;
            rarUrl.setScheme("file");
            rarUrl.setFragment("");
            QString rarPath = rarUrl.toLocalFile();

            return new RARImageSource(rarPath, imageName);
        } else {
            // Unsupported image format
            return 0;
        }
    } else {
        return 0;
    }
}

ImageSource *RARImageSourceFactory::createSingle(const QString &path)
{
    Q_UNUSED(path);
    Q_UNIMPLEMENTED();
    return 0;
}


QList<ImageSource *> RARImageSourceFactory::createMultiple(const QUrl &url)
{
    QList<ImageSource *> imageSources;

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
            QStringList fileNameList = rar.fileNameList();

            foreach(const QString &name, fileNameList) {
                QUrl imageUrl = url;
                imageUrl.setFragment(name);

                ImageSource *source = createSingle(imageUrl);
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
    Q_UNUSED(path);
    Q_UNIMPLEMENTED();
    return QList<ImageSource *>();
}
