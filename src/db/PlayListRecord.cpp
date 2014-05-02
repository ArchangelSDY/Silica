#include "LocalDatabase.h"
#include "LocalPlayListRecord.h"
#include "PlayList.h"
#include "PlayListRecord.h"
#include "RemotePlayListRecord.h"

PlayListRecord::PlayListRecord(const QString &name,
                               const QString &coverPath,
                               PlayList *playList,
                               QObject *parent) :
    QObject(parent) ,
    m_id(PlayListRecord::EMPTY_ID) ,
    m_name(name) ,
    m_coverPath(coverPath) ,
    m_count(PlayListRecord::EMPTY_COUNT) ,
    m_coverIndex(PlayListRecord::EMPTY_COVER_INDEX) ,
    m_playList(playList)
{
}

int PlayListRecord::coverIndex()
{
    if (m_coverIndex == PlayListRecord::EMPTY_COVER_INDEX) {
        PlayList *pl = playList();
        for (int i = 0; i < pl->count(); ++i) {
            if (pl->at(i)->thumbnailPath() == m_coverPath) {
                m_coverIndex = i;
                break;
            }
        }
    }

    return m_coverIndex;
}

bool PlayListRecord::save()
{
    bool ok = false;
    if (m_id == PlayListRecord::EMPTY_ID) {
        ok = LocalDatabase::instance()->insertPlayListRecord(this);
    } else {
        ok = LocalDatabase::instance()->updatePlayListRecord(this);
    }

    emit saved();
    return ok;
}

bool PlayListRecord::remove()
{
    return LocalDatabase::instance()->removePlayListRecord(this);
}

QList<PlayListRecord *> PlayListRecord::all()
{
    return LocalDatabase::instance()->queryPlayListRecords();
}

PlayListRecord *PlayListRecord::create(PlayListType type, const QString &name,
                                       const QString &coverPath)
{
    if (type == PlayListRecord::LocalPlayList) {
        return new LocalPlayListRecord(name, coverPath);
    } else if (type == PlayListRecord::RemotePlayList) {
        return new RemotePlayListRecord(name, coverPath);
    } else {
        return 0;
    }
}
