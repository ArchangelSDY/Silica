#ifndef REMOTEPLAYLISTRECORD_H
#define REMOTEPLAYLISTRECORD_H

#include "ImagesQuery.h"
#include "PlayListRecord.h"

class RemotePlayListRecord : public PlayListRecord
{
    Q_OBJECT
public:
    explicit RemotePlayListRecord(const QString &name, const QString &coverPath,
                                  PlayList *playList = 0, QObject *parent = 0);

    PlayList *playList();

private slots:
    void gotPlayList();

private:
    ImagesQuery *m_query;
};

#endif // REMOTEPLAYLISTRECORD_H
