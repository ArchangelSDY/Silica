#ifndef RANKINGFILTERMENUMANAGER_H
#define RANKINGFILTERMENUMANAGER_H

#include <QMenu>
#include <QObject>
#include <QSet>

class AbstractPlayListFilter;
class PlayList;

class RankFilterMenuManager : public QObject
{
    Q_OBJECT
public:
    explicit RankFilterMenuManager(QSharedPointer<PlayList> playList, QObject *parent = 0);
    ~RankFilterMenuManager();

    QMenu *menu();

private slots:
    void selectionChanged(bool selected);

private:
    QSharedPointer<PlayList> m_playList;
    QSet<int> m_unselectedRanks;
    QMenu *m_menu;
};

#endif // RANKINGFILTERMENUMANAGER_H
