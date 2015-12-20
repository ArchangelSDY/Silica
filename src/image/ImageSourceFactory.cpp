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

bool ImageSourceFactory::requestPassword(QString &password)
{
    if (m_mgr->m_client) {
        return m_mgr->m_client->requestPassword(password);
    } else {
        return false;
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
