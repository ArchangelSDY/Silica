#include "ImageSource.h"

#include <QCryptographicHash>
#include <QDir>
#include <QFileInfo>
#include <QSettings>
#include <QTextStream>
#include <QUrl>

#include "deps/quazip/quazip/quazipfile.h"
#include "GlobalConfig.h"

ImageSource::ImageSource(ImageSourceFactory *factory) :
    m_factory(factory)
{
}

ImageSource::~ImageSource()
{
    m_device.clear();
}

QString ImageSource::searchRealPath(QString path)
{
    QFileInfo file(path);
    if (file.exists()) {
        return path;
    }

    foreach (const QString &dir, GlobalConfig::instance()->searchDirs()) {
        QString realPath = dir + path;
        file = QFileInfo(realPath);
        if (file.exists()) {
            return realPath;
        }
    }

    return QString();
}

void ImageSource::close()
{
    if (!m_device.isNull()) {
        m_device->close();
        m_device.clear();
    }
}
