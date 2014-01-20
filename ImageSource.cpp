#include <QCryptographicHash>
#include <QFileInfo>
#include <QUrl>
#include <quazipfile.h>

#include "ImageSource.h"

ImageSource::ImageSource(QUrl url) :
    m_url(url) ,
    m_device(0)
{
}

const QByteArray ImageSource::hash() const
{
    return QCryptographicHash::hash(
        QByteArray(m_url.url().toUtf8()), QCryptographicHash::Sha1).toHex();
}

const QString ImageSource::name() const
{
    // TODO: init name in constructor
    if (m_url.scheme() == "zip") {
        return m_url.fragment();
    } else {
        QString path = m_url.toLocalFile();
        QFileInfo file(path);
        return file.fileName();
    }
}

bool ImageSource::open()
{
    if (!m_url.isValid() || m_url.isEmpty()) {
        return false;
    }

    if (m_url.scheme() == "file") {
        QString imagePath = m_url.toLocalFile();

        m_device.reset(new QFile(imagePath));
    } else if (m_url.scheme() == "zip") {
        QString imageName = m_url.fragment();

        QUrl zipUrl = m_url;
        zipUrl.setScheme("file");
        zipUrl.setFragment("");
        QString zipPath = zipUrl.toLocalFile();

        m_device.reset(new QuaZipFile(zipPath, imageName));
    }

    return m_device->open(QIODevice::ReadOnly);
}

void ImageSource::close()
{
    if (m_device) {
        m_device->close();
    }
}
