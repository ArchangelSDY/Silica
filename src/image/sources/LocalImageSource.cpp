#include <QCryptographicHash>
#include <QFileInfo>
#include <QTextStream>
#include <QUrl>

#include "LocalImageSource.h"

LocalImageSource::LocalImageSource(QString path)
{
    QString realPath = searchRealPath(path);
    if (!realPath.isEmpty()) {
        QFileInfo info(realPath);
        m_path = path;
        m_name = info.fileName();
    }

    // Compute hash
    QString hashStr;
    QTextStream(&hashStr) << "local#" << m_path << "#" << m_name;
    m_hash = QCryptographicHash::hash(
        QByteArray(hashStr.toUtf8()), QCryptographicHash::Sha1).toHex();

    // Compute url
    m_url = QUrl::fromLocalFile(m_path);
}

bool LocalImageSource::open()
{
    m_device.reset(new QFile(m_path));
    return m_device->open(QIODevice::ReadOnly);
}

bool LocalImageSource::copy(const QString &destPath)
{
    return QFile::copy(m_path, destPath);
}
