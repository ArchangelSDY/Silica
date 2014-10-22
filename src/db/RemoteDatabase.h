#ifndef REMOTEDATABASE_H
#define REMOTEDATABASE_H

#include <QNetworkDiskCache>
#include <QNetworkReply>
#include <QUrl>

#include "Image.h"
#include "MultiPageReplyIterator.h"

class RemoteDatabase
{
public:
    virtual ~RemoteDatabase() {}

    static RemoteDatabase *instance();

    virtual MultiPageReplyIterator *queryImagesByTag(const QString &tag) = 0;
    virtual QNetworkReply *addImageToAlbum(ImagePtr image,
                                           const QString &album) = 0;

protected:
    RemoteDatabase();

    virtual QNetworkRequest *prepareRequest(QNetworkRequest *req);
    virtual MultiPageReplyIterator *getMultiPage(QUrl url);

    QNetworkAccessManager m_manager;

private:
    static RemoteDatabase *s_instance;

    QNetworkDiskCache m_cache;
};

#endif // REMOTEDATABASE_H
