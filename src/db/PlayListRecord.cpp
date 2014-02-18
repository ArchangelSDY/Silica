#include "PlayListRecord.h"

PlayListRecord::PlayListRecord(const QString &name,
                               const QString &coverPath,
                               PlayList *playList) :
    m_name(name) ,
    m_coverPath(coverPath) ,
    m_playList(playList)
{
}

PlayList *PlayListRecord::playList()
{
    if (!m_playList) {
        m_playList = new PlayList();

        // TODO: Query image urls
    }

    return m_playList;
}
