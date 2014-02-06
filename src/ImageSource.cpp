#include <QCryptographicHash>
#include <QDir>
#include <QFileInfo>
#include <QSettings>
#include <QTextStream>
#include <QUrl>
#include <quazipfile.h>

#include "GlobalConfig.h"
#include "ImageSource.h"
#include "LocalImageSource.h"
#include "ZipImageSource.h"

ImageSource *ImageSource::create(QUrl url)
{
    if (url.scheme() == "file") {
        return new LocalImageSource(url.toLocalFile());
    } else if (url.scheme() == "zip") {
        QString imageName = url.fragment();

        QUrl zipUrl = url;
        zipUrl.setScheme("file");
        zipUrl.setFragment("");
        QString zipPath = zipUrl.toLocalFile();

        return new ZipImageSource(zipPath, imageName);
    } else {
        return 0;
    }
}

ImageSource *ImageSource::create(QString path)
{
    return new LocalImageSource(path);
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
