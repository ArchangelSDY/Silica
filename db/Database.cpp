#include "Database.h"

Database::Database(QObject *parent) :
    QObject(parent)
{
    m_cache.setCacheDirectory("netcache");
    m_net.setCache(&m_cache);
    connect(&m_net, SIGNAL(finished(QNetworkReply*)), this, SLOT(reqFinish(QNetworkReply*)));
}
