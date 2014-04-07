#include "LocalDatabase.h"
#include "LocalPlayListRecord.h"
#include "PlayList.h"

LocalPlayListRecord::LocalPlayListRecord(const QString &name,
                               const QString &coverPath,
                               PlayList *playList,
                               QObject *parent) :
    PlayListRecord(name, coverPath, playList, parent)
{
    m_type = PlayListRecord::LocalPlayList;
}

PlayList *LocalPlayListRecord::playList()
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
