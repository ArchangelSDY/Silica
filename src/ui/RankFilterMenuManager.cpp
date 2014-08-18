#include "ImageRank.h"
#include "NotEqualRankFilter.h"
#include "PlayList.h"
#include "RankFilterMenuManager.h"

RankFilterMenuManager::RankFilterMenuManager(PlayList **playList,
                                             QObject *parent) :
    QObject(parent) ,
    m_playList(playList) ,
    m_menu(0)
{
    m_menu = new QMenu(tr("Rank"));

    for (int i = ImageRank::MIN_VALUE; i <= ImageRank::MAX_VALUE; ++i) {
        QAction *rank = m_menu->addAction(
            QString("Rank %1").arg(i), this, SLOT(selectionChanged(bool)));
        rank->setData(QVariant(i));
        rank->setCheckable(true);

        if (i >= ImageRank::MIN_SHOWN_VALUE) {
            rank->setChecked(true);
        } else {
            m_unselectedRanks.insert(i);
        }
    }
}

RankFilterMenuManager::~RankFilterMenuManager()
{
    delete m_menu;
}

QMenu *RankFilterMenuManager::menu()
{
    return m_menu;
}

void RankFilterMenuManager::selectionChanged(bool selected)
{
    QAction *action = static_cast<QAction *>(QObject::sender());
    int rank = action->data().toInt();
    if (!selected) {
        m_unselectedRanks.insert(rank);
    } else {
        m_unselectedRanks.remove(rank);
    }

    if (m_playList && *m_playList) {
        AbstractPlayListFilter *filter = 0;
        foreach (const int &rank, m_unselectedRanks) {
            filter = new NotEqualRankFilter(rank, filter);
        }
        (*m_playList)->setFilter(filter);
    }
}
