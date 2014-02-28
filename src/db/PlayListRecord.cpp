#include "LocalDatabase.h"
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
