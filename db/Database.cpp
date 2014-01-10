#include "Database.h"

Database::Database(QObject *parent) :
    QObject(parent)
{
    connect(&m_net, SIGNAL(finished(QNetworkReply*)), this, SLOT(reqFinish(QNetworkReply*)));
}
