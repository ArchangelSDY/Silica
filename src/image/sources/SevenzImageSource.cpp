#include <QCryptographicHash>
#include <QFile>
#include <QTextStream>
#include <QThread>
#include <QUrl>

#include "Qt7zPackage.h"

#include "SevenzImageSource.h"

FrequencyCache<QString, QSharedPointer<Qt7zPackage> >
    SevenzImageSource::m_packageCache(QThread::idealThreadCount());

QMutex SevenzImageSource::m_mutex;

SevenzImageSource::SevenzImageSource(QString packagePath, QString imageName)
{
    m_packagePath = searchRealPath(packagePath);
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

    // Compute hash
    // Thread id should be taken into account because we internally take
    // advantage of `outBuffer` as cache and it SHOULD NOT be shared among
    // threads.
    QString hash;
    QTextStream hashBuilder(&hash);
    long threadId = reinterpret_cast<long>(QThread::currentThreadId());
    hashBuilder << QString::number(threadId, 16)
                << "#" << m_packagePath;

    m_mutex.lock();
    QSharedPointer<Qt7zPackage> pkg = m_packageCache.find(hash);
    if (pkg.isNull()) {
        pkg = QSharedPointer<Qt7zPackage>(new Qt7zPackage(m_packagePath));
        pkg->open();
        if (pkg->isOpen()) {
            m_packageCache.insert(hash, pkg);
        }
    }
    m_mutex.unlock();

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
