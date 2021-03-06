#include <QCryptographicHash>
#include <QFile>
#include <QFileInfo>
#include <QMutexLocker>
#include <QSharedPointer>
#include <QTextStream>
#include <QThread>
#include <QUrl>

#include "deps/Qt7z/Qt7z/Qt7zPackage.h"
#include "SevenzImageSource.h"
#include "SevenzImageSourceFactory.h"

class SevenzImageSourceClient : public Qt7zPackage::Client
{
public:
    SevenzImageSourceClient(const QString &password) :
        m_password(password)
    {
    }

    virtual void openPasswordRequired(QString & password) override
    {
        password = m_password;
    }

    virtual void extractPasswordRequired(QString & password) override
    {
        password = m_password;
    }

private:
    QString m_password;
};

SevenzImageSource::SevenzImageSource(ImageSourceFactory *factory,
                                     QString packagePath,
                                     QString imageName,
                                     QString password) :
    ImageSource(factory) ,
    m_packagePath(packagePath) ,
    m_password(password)
{
    m_name = imageName;

    // Compute hash
    QString hashStr;
    QTextStream(&hashStr) <<  "sevenz#" << m_packagePath << "#" << m_name;
    m_hash = QCryptographicHash::hash(
        QByteArray(hashStr.toUtf8()), QCryptographicHash::Sha1).toHex();

    // Compute url
    m_url = QUrl::fromLocalFile(m_packagePath);
    m_url.setScheme("sevenz");
    m_url.setFragment(m_name);
}

bool SevenzImageSource::open()
{
    QBuffer *buffer = new QBuffer();
    buffer->open(QIODevice::ReadWrite);

    // TODO: Request password if required

    // Compute hash
    // Thread id should be taken into account because we internally take
    // advantage of `outBuffer` as cache and it SHOULD NOT be shared among
    // threads.
    QString hash;
    QTextStream hashBuilder(&hash);
    long long threadId = reinterpret_cast<long long>(QThread::currentThreadId());
    hashBuilder << QString::number(threadId, 16)
                << "#" << m_packagePath;

    SevenzImageSourceFactory *factory =
        static_cast<SevenzImageSourceFactory *>(m_factory);
    QSharedPointer<Qt7zPackage> pkg;
    {
        QMutexLocker(&factory->m_mutex);
        pkg = factory->m_packageCache.find(hash);
        if (pkg.isNull()) {
            pkg = QSharedPointer<Qt7zPackage>(new Qt7zPackage(m_packagePath), [](Qt7zPackage *pkg) {
                delete pkg->client();
                delete pkg;
            });
            pkg->setClient(new SevenzImageSourceClient(m_password));
            pkg->open();
            if (pkg->isOpen()) {
               factory->m_packageCache.insert(hash, pkg);
            } else {
                if (!m_password.isEmpty()) {
                    // Password is present but open failed, consider it wrong password
                    passwordRejected(m_packagePath);
                }
            }
        }
    }

    if (!pkg->isOpen()) {
        delete buffer;
        return false;
    }

    if (!pkg->extractFile(m_name, buffer)) {
        delete buffer;
        return false;
    }

    buffer->seek(0);
    m_device.reset(buffer);

    return true;
}

bool SevenzImageSource::exists()
{
    return QFileInfo::exists(m_packagePath);
}

bool SevenzImageSource::copy(const QString &destPath)
{
    Qt7zPackage pkg(m_packagePath);
    if (!pkg.open()) {
        return false;
    }

    QFile *file = new QFile(destPath);
    if (!file->open(QIODevice::WriteOnly)) {
        delete file;
        return false;
    }

    if (!pkg.extractFile(m_name, file)) {
        pkg.close();
        file->close();
        delete file;
        return false;
    }

    file->close();
    delete file;
    return true;
}
