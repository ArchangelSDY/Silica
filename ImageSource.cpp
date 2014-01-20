#include <QCryptographicHash>
#include <QFileInfo>
#include <QTextStream>
#include <QUrl>
#include <quazipfile.h>

#include "ImageSource.h"

ImageSource::ImageSource(QUrl url) :
    m_sourceFormat(NoneImage) ,
    m_device(0)
{
    if (url.scheme() == "file") {
        initLocalImage(url.toLocalFile());
    } else if (url.scheme() == "zip") {
        m_name = url.fragment();

        QUrl zipUrl = url;
        zipUrl.setScheme("file");
        zipUrl.setFragment("");
        m_zipPath = zipUrl.toLocalFile();

        m_sourceFormat = LocalZip;
    }
}

ImageSource::ImageSource(QString path) :
    m_sourceFormat(NoneImage) ,
    m_device(0)
{
    initLocalImage(path);
}

void ImageSource::initLocalImage(QString path)
{
    QFileInfo file(path);
    if (file.exists()) {
        m_imagePath = path;
        m_name = file.fileName();
        m_sourceFormat = LocalImage;
    }
}

const QByteArray ImageSource::hash()
{
    if (m_hash.isEmpty()) {
        computeHash();
    }

    return m_hash;
}

bool ImageSource::open()
{
    if (m_sourceFormat == NoneImage) {
        return false;
    }

    if (m_sourceFormat == LocalImage) {
        m_device.reset(new QFile(m_imagePath));
    } else if (m_sourceFormat == LocalZip) {
        m_device.reset(new QuaZipFile(m_zipPath, m_name));
    }

    return m_device->open(QIODevice::ReadOnly);
}

void ImageSource::close()
{
    if (m_device) {
        m_device->close();
    }
}

void ImageSource::computeHash()
{
    QString totalPath;
    QTextStream(&totalPath) << m_imagePath << "#" << m_zipPath;
    m_hash = QCryptographicHash::hash(
        QByteArray(totalPath.toUtf8()), QCryptographicHash::Sha1).toHex();
}
