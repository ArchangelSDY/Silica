#include <QCoreApplication>
#include <QNetworkReply>
#include <QUrlQuery>

#include "AbstractQuery.h"

AbstractQuery::AbstractQuery(QObject *parent) :
    QObject(parent) ,
    m_page(1)   // Page id starts from 1
{
    m_cache.setCacheDirectory(
        QCoreApplication::applicationDirPath() + "/netcache");
    m_manager.setCache(&m_cache);
    connect(&m_manager, SIGNAL(finished(QNetworkReply*)),
        this, SLOT(reqFinish(QNetworkReply*)));
}

AbstractQuery::~AbstractQuery()
{
}

void AbstractQuery::reqFinish(QNetworkReply *reply)
{
    reply->deleteLater();

    if (reply->request() == m_request &&
        reply->error() == QNetworkReply::NoError) {
        parseReply(reply);
    }
}

void AbstractQuery::start()
{
    QUrl url = m_request.url();
    QUrlQuery query(url);
    query.removeAllQueryItems("page");
    query.addQueryItem("page", QString::number(m_page));
    url.setQuery(query);
    m_request.setUrl(url);

    m_manager.get(m_request);
}

void AbstractQuery::queryNextPage()
{
    m_page++;
    start();
}
