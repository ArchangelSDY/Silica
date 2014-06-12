#include "ImageRank.h"
#include "Navigator.h"
#include "NotEqualRankFilter.h"
#include "RankFilterMenuManager.h"

RankFilterMenuManager::RankFilterMenuManager(Navigator **navigator,
                                             QObject *parent) :
    QObject(parent) ,
    m_navigator(navigator) ,
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

    if (*m_navigator && (*m_navigator)->playList()) {
        AbstractPlayListFilter *filter = 0;
        foreach (const int &rank, m_unselectedRanks) {
            filter = new NotEqualRankFilter(rank, filter);
        }
        PlayList *pl = (*m_navigator)->playList();
        pl->setFilter(filter);
    }
}
