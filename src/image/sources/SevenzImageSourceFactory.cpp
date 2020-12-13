#include <QFileInfo>
#include <QImageReader>
#include <QThread>
#include <QUrl>

#include "deps/Qt7z/Qt7z/Qt7zFileInfo.h"
#include "deps/Qt7z/Qt7z/Qt7zPackage.h"
#include "SevenzImageSource.h"
#include "SevenzImageSourceFactory.h"

class SevenzImageSourceFactory::Client : public Qt7zPackage::Client
{
public:
    Client(SevenzImageSourceFactory *factory, const QString &packagePath) :
        m_factory(factory),
        m_packagePath(packagePath)
    {
    }

    virtual void openPasswordRequired(QString & password) override
    {
        m_factory->requestPassword(m_packagePath, password);
        m_password = password;
    }

    virtual void extractPasswordRequired(QString & password) override
    {
        Q_UNREACHABLE();
    }

    QString password() const
    {
        return m_password;
    }

private:
    SevenzImageSourceFactory *m_factory;
    QString m_packagePath;
    QString m_password;
};

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
    return createSingle(url, QString());
}

ImageSource *SevenzImageSourceFactory::createSingle(const QUrl &url, const QString &password)
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
            QString realSevenzPath = findRealPath(sevenzPath);

            return new SevenzImageSource(this, realSevenzPath, imageName, password);
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

    if (!isValidFileName(url.path())) {
        return imageSources;
    }

    if (url.hasFragment()) {
        ImageSource *source = createSingle(url);
        if (source) {
            imageSources << source;
        }
    } else {
        // Add all files in the 7z package
        QUrl fileUrl = url;
        fileUrl.setScheme("file");
        QString packagePath = fileUrl.toLocalFile();
        QString realPackagePath = findRealPath(packagePath);

        Qt7zPackage pkg(realPackagePath);
        Client client(this, realPackagePath);
        pkg.setClient(&client);
        bool success = pkg.open();

        if (!success) {
            if (!client.password().isEmpty()) {
                // Password is requested but open failed, consider it a wrong password
                passwordRejected(realPackagePath);
            }

            return imageSources;
        }

        QStringList fileNameList = pkg.getFileNameList();
        QString password = client.password();

        if (password.isEmpty() && !fileNameList.isEmpty()) {
            Qt7zFileInfo firstFileInfo = pkg.fileInfoList().first();
            if (firstFileInfo.isEncrypted) {
                if (!requestPassword(realPackagePath, password)) {
                    return imageSources;
                }
            }
        }

        if (!password.isEmpty()) {
            passwordAccepted(realPackagePath, password);
        }

        foreach(const QString &name, fileNameList) {
            QUrl imageUrl = url;
            imageUrl.setFragment(name);

            ImageSource *source = createSingle(imageUrl, password);
            if (source) {
                imageSources << source;
            }
        }

        pkg.close();
    }

    return imageSources;
}

QList<ImageSource *> SevenzImageSourceFactory::createMultiple(const QString &path)
{
    QUrl url = QUrl::fromLocalFile(path);
    url.setScheme(urlScheme());
    return createMultiple(url);
}

void SevenzImageSourceFactory::clearCache()
{
    m_mutex.lock();
    m_packageCache.clear();
    m_mutex.unlock();
}
