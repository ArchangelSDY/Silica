#ifndef ASUNADATABASE_H
#define ASUNADATABASE_H

#include <QSettings>

#include "RemoteDatabase.h"

class AsunaDatabase : public RemoteDatabase
{
public:
    AsunaDatabase();
    AsunaDatabase(const QUrl &baseUrl);

    virtual MultiPageReplyIterator *queryImagesByTag(const QString &tag);
    virtual QNetworkReply *addImageToAlbum(ImagePtr image,
                                           const QString &album);

private:
    QString m_baseUrl;
};

#endif // ASUNADATABASE_H
