#include "ImagesQuery.h"
#include "PlayList.h"
#include "RemotePlayListRecord.h"

RemotePlayListRecord::RemotePlayListRecord(const QString &name,
                                           const QString &coverPath,
                                           QObject *parent) :
    PlayListRecord(name, coverPath, 0, parent) ,
    m_query(0)
{
    m_type = PlayListRecord::RemotePlayList;
}

PlayList *RemotePlayListRecord::playList()
{
    if (!m_playList) {
        m_playList = new PlayList();
        m_playList->setRecord(this);

        // Start a query
        if (!m_query) {
            m_query = new ImagesQuery(m_name, this);
            connect(m_query, SIGNAL(gotContent()), this, SLOT(gotPlayList()));
            m_query->start();
        }
    }

    return m_playList;
}

void RemotePlayListRecord::gotPlayList()
{
    Q_ASSERT(m_query);
    Q_ASSERT(m_playList);

    PlayList *toBeAppended = m_query->playList();
    if (toBeAppended) {
        m_playList->append(toBeAppended);
        delete toBeAppended;
    }
}
