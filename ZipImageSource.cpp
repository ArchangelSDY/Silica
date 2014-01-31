#include <QCryptographicHash>
#include <QTextStream>
#include <quazipfile.h>

#include "ZipImageSource.h"

ZipImageSource::ZipImageSource(QString zipPath, QString imageName)
{
    m_zipPath = searchRealPath(zipPath);
    m_name = imageName;

    // Compute hash
    QString hashStr;
    QTextStream(&hashStr) <<  "zip#" << m_zipPath << "#" << m_name;
    m_hash = QCryptographicHash::hash(
        QByteArray(hashStr.toUtf8()), QCryptographicHash::Sha1).toHex();
}

bool ZipImageSource::open()
{
    m_device.reset(new QuaZipFile(m_zipPath, m_name));
    return m_device->open(QIODevice::ReadOnly);
}
