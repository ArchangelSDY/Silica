#include <QRegExp>
#include <QStringList>

#include "ImageSourceFactory.h"
#include "ImageSourceManager.h"

ImageSourceFactory::ImageSourceFactory(ImageSourceManager *mgr,
                                       QObject *parent) :
    QObject(parent) ,
    m_mgr(mgr)
{
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
