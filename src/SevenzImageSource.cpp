#include <QCryptographicHash>
#include <QTextStream>

#include "Qt7zPackage.h"

#include "SevenzImageSource.h"

SevenzImageSource::SevenzImageSource(QString packagePath, QString imageName)
{
    m_packagePath = searchRealPath(packagePath);
    m_imageName = imageName;

    // Compute hash
    QString hashStr;
    QTextStream(&hashStr) <<  "sevenz#" << m_packagePath << "#" << m_name;
    m_hash = QCryptographicHash::hash(
        QByteArray(hashStr.toUtf8()), QCryptographicHash::Sha1).toHex();
}

bool SevenzImageSource::open()
{
    QBuffer *buffer = new QBuffer();
    buffer->open(QIODevice::ReadWrite);

    Qt7zPackage pkg(m_packagePath);
    if (!pkg.open()) {
        delete buffer;
        return false;
    }

    if (!pkg.extractFile(m_imageName, buffer)) {
        pkg.close();
        delete buffer;
        return false;
    }

    buffer->seek(0);
    m_device.reset(buffer);
    return true;
}

bool SevenzImageSource::copy(const QString &destPath)
{
    Q_UNIMPLEMENTED();
    return true;
}
