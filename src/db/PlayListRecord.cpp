#include "LocalDatabase.h"
#include "PlayList.h"
#include "PlayListRecord.h"

PlayListRecord::PlayListRecord(const QString &name,
                               const QString &coverPath,
                               PlayList *playList) :
    m_id(PlayListRecord::EMPTY_ID) ,
    m_name(name) ,
    m_coverPath(coverPath) ,
    m_playList(playList)
{
}

PlayList *PlayListRecord::playList()
{
    if (!m_playList) {
        m_playList = new PlayList();
        m_playList->setRecord(this);

        QStringList imageUrls =
            LocalDatabase::instance()->queryImageUrlsForPlayList(m_id);
        foreach (const QString &imageUrl, imageUrls) {
            m_playList->addPath(QUrl(imageUrl));
        }
    }

    return m_playList;
}

QList<PlayListRecord *> PlayListRecord::all()
{
    return LocalDatabase::instance()->queryPlayListRecords();
}

bool PlayListRecord::save()
{
    if (m_id == PlayListRecord::EMPTY_ID) {
        return LocalDatabase::instance()->insertPlayListRecord(this);
    } else {
        return LocalDatabase::instance()->updatePlayListRecord(this);
    }
}

bool PlayListRecord::remove()
{
    return LocalDatabase::instance()->removePlayListRecord(this);
}
