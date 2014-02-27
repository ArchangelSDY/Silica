#include "LocalDatabase.h"
#include "PlayListRecord.h"

PlayListRecord::PlayListRecord(const QString &name,
                               const QString &coverPath,
                               PlayList *playList) :
    m_id(0) ,
    m_name(name) ,
    m_coverPath(coverPath) ,
    m_playList(playList)
{
}

PlayList *PlayListRecord::playList()
{
    if (!m_playList) {
        m_playList = new PlayList();

        QStringList imageUrls =
            LocalDatabase::instance()->queryImageUrlsForPlayList(m_id);
        foreach (const QString &imageUrl, imageUrls) {
            m_playList->addPath(QUrl(imageUrl));
        }
    }

    return m_playList;
}

QList<PlayListRecord> PlayListRecord::fromLocalDatabase()
{
    return LocalDatabase::instance()->queryPlayListRecords();
}

bool PlayListRecord::saveToLocalDatabase()
{
    return LocalDatabase::instance()->insertPlayListRecord(this);
}

bool PlayListRecord::remove()
{
    return LocalDatabase::instance()->removePlayListRecord(this);
}
