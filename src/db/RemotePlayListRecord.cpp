#include "ImagesQuery.h"
#include "PlayList.h"
#include "RemotePlayListRecord.h"

RemotePlayListRecord::RemotePlayListRecord(const QString &name,
                                           const QString &coverPath,
                                           PlayList *playList,
                                           QObject *parent) :
    PlayListRecord(name, coverPath, playList, parent) ,
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

    m_playList->append(m_query->playList());
    // Have to emit signals here because PlayList (as a QList) is not possible
    // to know items change
    // FIXME: Maybe final solution is to rewrite PlayList to make it contains
    // QList instead of deriving from it.
    emit m_playList->eventEmitter()->itemsAppended();
}
