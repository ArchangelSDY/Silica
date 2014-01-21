#include <QCryptographicHash>
#include <QDir>
#include <QFileInfo>
#include <QSettings>
#include <QTextStream>
#include <QUrl>
#include <quazipfile.h>

#include "GlobalConfig.h"
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
        m_zipPath = searchRealPath(zipUrl.toLocalFile());

        if (!m_zipPath.isEmpty()) {
            m_sourceFormat = LocalZip;
        } else {
            m_sourceFormat = NoneImage;
        }
    }

    computeHash();
}

ImageSource::ImageSource(QString path) :
    m_sourceFormat(NoneImage) ,
    m_device(0)
{
    initLocalImage(path);
    computeHash();
}

void ImageSource::initLocalImage(QString path)
{
    QString realPath = searchRealPath(path);
    if (!realPath.isEmpty()) {
        QFileInfo info(realPath);
        m_imagePath = path;
        m_name = info.fileName();
        m_sourceFormat = LocalImage;
    }

}

QString ImageSource::searchRealPath(QString path)
{
    QFileInfo file(path);
    if (file.exists()) {
        return path;
    }

    foreach (const QString &dir, GlobalConfig::instance()->zipDirs()) {
        QString realPath = dir + path;
        file = QFileInfo(realPath);
        if (file.exists()) {
            return realPath;
        }
    }

    return QString();
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
    QTextStream(&totalPath) << m_imagePath << "#" << m_zipPath << "#" << m_name;
    m_hash = QCryptographicHash::hash(
        QByteArray(totalPath.toUtf8()), QCryptographicHash::Sha1).toHex();
}
