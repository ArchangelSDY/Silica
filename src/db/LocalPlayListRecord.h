#ifndef LOCALPLAYLISTRECORD_H
#define LOCALPLAYLISTRECORD_H

#include "PlayListRecord.h"

class LocalPlayListRecord : public PlayListRecord
{
    Q_OBJECT
public:
    explicit LocalPlayListRecord(const QString &name, const QString &coverPath,
        PlayList *playList = 0, QObject *parent = 0);

    PlayList *playList();
};

#endif // LOCALPLAYLISTRECORD_H
