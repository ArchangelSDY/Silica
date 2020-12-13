#include <QFileInfo>
#include <QRegExp>
#include <QStringList>

#include "GlobalConfig.h"
#include "ImageSourceFactory.h"
#include "ImageSourceManager.h"

ImageSourceFactory::ImageSourceFactory(ImageSourceManager *mgr,
                                       QObject *parent) :
    QObject(parent) ,
    m_mgr(mgr)
{
}

QString ImageSourceFactory::findRealPath(QString path)
{
    // Given path exists, should be exact real path
    QFileInfo file(path);
    if (file.exists()) {
        return path;
    }

    // Try all search directories
    for (const QString &dir : GlobalConfig::instance()->searchDirs()) {
        QString realPath = dir + path;
        file = QFileInfo(realPath);
        if (file.exists()) {
            return realPath;
        }
    }

    // Not found in search directories, leave path unchanged
    return path;
}

bool ImageSourceFactory::requestPassword(const QString &archivePath, QString &password)
{
    if (m_mgr->m_client) {
        return m_mgr->m_client->requestPassword(archivePath, password);
    } else {
        return false;
    }
}

void ImageSourceFactory::passwordAccepted(const QString &archivePath, const QString &password)
{
    if (m_mgr->m_client) {
        return m_mgr->m_client->passwordAccepted(archivePath, password);
    }
}

void ImageSourceFactory::passwordRejected(const QString &archivePath)
{
    if (m_mgr->m_client) {
        return m_mgr->m_client->passwordRejected(archivePath);
    }
}

bool ImageSourceFactory::isValidFileName(const QString &name)
{
    QStringList patterns = fileNamePattern().split(' ');
    foreach (const QString &pattern, patterns) {
        QRegExp reg(pattern, Qt::CaseInsensitive, QRegExp::Wildcard);
        if (reg.exactMatch(name)) {
            return true;
        }
    }

    return false;
}
