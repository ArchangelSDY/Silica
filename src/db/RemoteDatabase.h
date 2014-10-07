#ifndef REMOTEDATABASE_H
#define REMOTEDATABASE_H

#include <QNetworkDiskCache>
#include <QNetworkReply>
#include <QUrl>

#include "MultiPageReplyIterator.h"

class RemoteDatabase
{
public:
    static RemoteDatabase *instance();

    virtual MultiPageReplyIterator *queryImagesByTag(const QString &tag) = 0;

protected:
    RemoteDatabase();

    virtual QNetworkRequest *prepareRequest(QNetworkRequest *req);
    virtual MultiPageReplyIterator *getMultiPage(QUrl url);

private:
    static RemoteDatabase *s_instance;

    QNetworkAccessManager m_manager;
    QNetworkDiskCache m_cache;
};

#endif // REMOTEDATABASE_H
