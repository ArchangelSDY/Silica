#include "CGColleImageSource.h"

#include <QBuffer>
#include <QCryptographicHash>
#include <QFileInfo>
#include <QTextStream>

#include "CGColleReader.h"

CGColleImageSource::CGColleImageSource(ImageSourceFactory *factory,
                                       QString packagePath, QString imageName) :
    ImageSource(factory)
{
    m_packagePath = findRealPath(packagePath);
    m_name = imageName;

    // Compute hash
    QString hashStr;
    QTextStream(&hashStr) << "cgc#" << m_packagePath << "#" << m_name;
    m_hash = QCryptographicHash::hash(
        QByteArray(hashStr.toUtf8()), QCryptographicHash::Sha1).toHex();

    // Compute url
    m_url = QUrl::fromLocalFile(m_packagePath);
    m_url.setScheme("cgc");
    m_url.setFragment(m_name);
}

bool CGColleImageSource::open()
{
    CGColleReader package(m_packagePath);
    if (package.open()) {
        m_device.reset(new QBuffer());
        if (!m_device->open(QIODevice::ReadWrite)) {
            m_device.reset();
            return false;
        }

        m_device->write(package.read(m_name));
        m_device->reset();
        return true;
    } else {
        return false;
    }
}

bool CGColleImageSource::exists()
{
    return QFileInfo::exists(m_packagePath);
}

bool CGColleImageSource::copy(const QString &destPath)
{
    return false;
}