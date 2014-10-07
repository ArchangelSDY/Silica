#include <QUrlQuery>

#include "MultiPageReplyIterator.h"

MultiPageReplyIterator::MultiPageReplyIterator(QNetworkReply *reply,
                                               QObject *parent) :
    QObject(parent) ,
    m_reply(0)
{
    setReply(reply);
}

void MultiPageReplyIterator::setReply(QNetworkReply *reply)
{
    m_reply = reply;
    connect(m_reply, SIGNAL(finished()), this, SLOT(replyFinished()));
}

void MultiPageReplyIterator::replyFinished()
{
    if (m_reply->error() == QNetworkReply::NoError) {
        emit gotPage(m_reply);

        QNetworkAccessManager *mgr = m_reply->manager();
        QNetworkRequest nextReq = m_reply->request();

        QUrl nextUrl = nextReq.url();
        QUrlQuery nextQuery(nextUrl);

        QString pageItem = nextQuery.queryItemValue("page");
        int nextPage = pageItem.toInt() + 1;
        nextQuery.removeAllQueryItems("page");
        nextQuery.addQueryItem("page", QString::number(nextPage));

        nextUrl.setQuery(nextQuery);
        nextReq.setUrl(nextUrl);

        setReply(mgr->get(nextReq));
    } else {
        m_reply->deleteLater();
    }
}
