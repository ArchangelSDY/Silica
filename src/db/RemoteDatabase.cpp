#include <QCoreApplication>

#include "RemoteDatabase.h"

RemoteDatabase::RemoteDatabase(QObject *parent) :
    QObject(parent)
{
    m_cache.setCacheDirectory(
        QCoreApplication::applicationDirPath() + "/netcache");
    m_net.setCache(&m_cache);
    connect(&m_net, SIGNAL(finished(QNetworkReply*)),
        this, SLOT(reqFinish(QNetworkReply*)));
}
