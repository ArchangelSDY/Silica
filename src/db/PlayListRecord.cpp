#include "LocalDatabase.h"
#include "LocalPlayListRecord.h"
#include "PlayList.h"
#include "PlayListRecord.h"

PlayListRecord::PlayListRecord(const QString &name,
                               const QString &coverPath,
                               PlayList *playList,
                               QObject *parent) :
    QObject(parent) ,
    m_id(PlayListRecord::EMPTY_ID) ,
    m_name(name) ,
    m_coverPath(coverPath) ,
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

QList<PlayListRecord *> PlayListRecord::all()
{
    return LocalDatabase::instance()->queryPlayListRecords();
}

PlayListRecord *PlayListRecord::create(PlayListType type, const QString &name,
                                       const QString &coverPath)
{
    if (type == PlayListRecord::LocalPlayList) {
        return new LocalPlayListRecord(name, coverPath);
    } else {
        return 0;
    }
}
