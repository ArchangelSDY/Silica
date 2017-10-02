#include "CGColleImageSource.h"

#include <QBuffer>
#include <QCryptographicHash>
#include <QFileInfo>
#include <QTextStream>

#include "CGColleReader.h"

CGColleImageSource::CGColleImageSource(ImageSourceFactory *factory,
                                       QString packagePath, QString imageName, CGColleReader::ImageReader *reader) :
    ImageSource(factory) ,
    m_reader(reader)
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
    Q_ASSERT(!m_reader.isNull());
    m_device.reset(new QBuffer());
    if (!m_device->open(QIODevice::ReadWrite)) {
        m_device.reset();
        return false;
    }

    m_device->write(m_reader->read());
    m_device->reset();
    return true;
}

bool CGColleImageSource::exists()
{
    return QFileInfo::exists(m_packagePath);
}

bool CGColleImageSource::copy(const QString &destPath)
{
    return false;
}