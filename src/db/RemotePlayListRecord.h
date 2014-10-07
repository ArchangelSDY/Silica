#ifndef REMOTEPLAYLISTRECORD_H
#define REMOTEPLAYLISTRECORD_H

#include "MultiPageReplyIterator.h"
#include "PlayListRecord.h"

class RemotePlayListRecord : public PlayListRecord
{
    Q_OBJECT
public:
    explicit RemotePlayListRecord(const QString &name,
                                  const QString &coverPath = QString(),
                                  QObject *parent = 0);
    ~RemotePlayListRecord();

    PlayList *playList();

private slots:
    void gotReplyPage(QNetworkReply *reply);

private:
    MultiPageReplyIterator *m_replyIter;
};

#endif // REMOTEPLAYLISTRECORD_H
