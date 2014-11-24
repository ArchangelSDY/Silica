#include <QCryptographicHash>
#include <QDir>
#include <QFileInfo>
#include <QSettings>
#include <QTextStream>
#include <QUrl>
#include <quazipfile.h>

#include "GlobalConfig.h"
#include "ImageSource.h"

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

    foreach (const QString &dir, GlobalConfig::instance()->zipDirs()) {
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
