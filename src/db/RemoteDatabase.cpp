#include <QCoreApplication>
#include <QNetworkConfiguration>

#include "AsunaDatabase.h"
#include "RemoteDatabase.h"

RemoteDatabase *RemoteDatabase::s_instance = 0;

RemoteDatabase *RemoteDatabase::instance()
{
    if (!s_instance) {
        s_instance = new AsunaDatabase();
    }

    return s_instance;
}

RemoteDatabase::RemoteDatabase()
{
    m_cache.setCacheDirectory(
        QCoreApplication::applicationDirPath() + "/netcache");
    m_manager.setCache(&m_cache);
}

QNetworkRequest *RemoteDatabase::prepareRequest(QNetworkRequest *req)
{
    // If offline, use cache.
    if (m_manager.configuration().state().testFlag(
            QNetworkConfiguration::Active)) {
        req->setAttribute(QNetworkRequest::CacheLoadControlAttribute,
            QNetworkRequest::PreferNetwork);
    } else {
        req->setAttribute(QNetworkRequest::CacheLoadControlAttribute,
            QNetworkRequest::AlwaysCache);
    }

    return req;
}

MultiPageReplyIterator *RemoteDatabase::getMultiPage(QUrl url)
{
    QNetworkRequest req(url);
    prepareRequest(&req);
    QNetworkReply *reply = m_manager.get(req);
    return new MultiPageReplyIterator(reply);
}
