#include <QBuffer>
#include <QCryptographicHash>
#include <QTextStream>
#include <QUrl>
#include <quazipfile.h>

#include "ZipImageSource.h"

ZipImageSource::ZipImageSource(ImageSourceFactory *factory,
                               QString zipPath, QString imageName) :
    ImageSource(factory)
{
    m_zipPath = searchRealPath(zipPath);
    m_name = imageName;

    // Compute hash
    QString hashStr;
    QTextStream(&hashStr) <<  "zip#" << m_zipPath << "#" << m_name;
    m_hash = QCryptographicHash::hash(
        QByteArray(hashStr.toUtf8()), QCryptographicHash::Sha1).toHex();

    // Compute url
    m_url = QUrl::fromLocalFile(m_zipPath);
    m_url.setScheme("zip");
    m_url.setFragment(m_name);
}

bool ZipImageSource::open()
{
    QuaZipFile zipFile(m_zipPath, m_name);
    if (zipFile.open(QIODevice::ReadOnly)) {
        m_device.reset(new QBuffer());
        if (!m_device->open(QIODevice::ReadWrite)) {
            m_device.clear();
            return false;
        }

        m_device->write(zipFile.readAll());
        m_device->reset();
        return true;
    } else {
        return false;
    }
}

bool ZipImageSource::copy(const QString &destPath)
{
    QFile dest(destPath);
    if (!dest.open(QIODevice::WriteOnly)) {
        return false;
    }

    QuaZipFile src(m_zipPath, m_name);
    if (!src.open(QIODevice::ReadOnly)) {
        dest.close();
        return false;
    }

    dest.write(src.readAll());

    dest.close();
    src.close();

    return true;
}
