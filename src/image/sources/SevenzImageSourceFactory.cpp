#include <QFileInfo>
#include <QImageReader>
#include <QThread>
#include <QUrl>
#include <Qt7zPackage.h>

#include "SevenzImageSource.h"
#include "SevenzImageSourceFactory.h"

FrequencyCache<QString, QSharedPointer<Qt7zPackage> >
    SevenzImageSourceFactory::m_packageCache(QThread::idealThreadCount());

QMutex SevenzImageSourceFactory::m_mutex;

SevenzImageSourceFactory::SevenzImageSourceFactory(ImageSourceManager *mgr) :
    ImageSourceFactory(mgr)
{
}

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
        QFileInfo imageFile(imageName);
        if (QImageReader::supportedImageFormats().contains(
                imageFile.suffix().toUtf8())) {
            QUrl sevenzUrl = url;
            sevenzUrl.setScheme("file");
            sevenzUrl.setFragment("");
            QString sevenzPath = sevenzUrl.toLocalFile();

            return new SevenzImageSource(this, sevenzPath, imageName);
        } else {
            // Unsupported image format
            return 0;
        }
    } else {
        return 0;
    }
}

ImageSource *SevenzImageSourceFactory::createSingle(const QString &)
{
    return 0;
}

QList<ImageSource *> SevenzImageSourceFactory::createMultiple(const QUrl &url)
{
    QList<ImageSource *> imageSources;

    if (url.hasFragment()) {
        imageSources << createSingle(url);
    } else {
        // Add all files in the 7z package
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

                ImageSource *source = createSingle(imageUrl);
                if (source) {
                    imageSources << source;
                }
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

void SevenzImageSourceFactory::clearCache()
{
    m_mutex.lock();
    m_packageCache.clear();
    m_mutex.unlock();
}
