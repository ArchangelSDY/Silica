#include <QCryptographicHash>
#include <QTextStream>

#include "deps/QtRAR/src/qtrarfile.h"

#include "RARImageSource.h"

RARImageSource::RARImageSource(const QString &arcPath,
                               const QString &imageName)
{
    m_arcPath = searchRealPath(arcPath);
    m_name = imageName;

    // Compute hash
    QString hashStr;
    QTextStream(&hashStr) <<  "rar#" << m_arcPath << "#" << m_name;
    m_hash = QCryptographicHash::hash(
        QByteArray(hashStr.toUtf8()), QCryptographicHash::Sha1).toHex();

    // Compute url
    m_url = QUrl::fromLocalFile(m_arcPath);
    m_url.setScheme("rar");
    m_url.setFragment(m_name);
}

bool RARImageSource::open()
{
    m_device.reset(new QtRARFile(m_arcPath, m_name));
    return m_device->open(QIODevice::ReadOnly);
}

bool RARImageSource::copy(const QString &destPath)
{
    return false;
}
