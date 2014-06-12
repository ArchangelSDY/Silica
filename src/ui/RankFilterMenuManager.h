#ifndef RANKINGFILTERMENUMANAGER_H
#define RANKINGFILTERMENUMANAGER_H

#include <QMenu>
#include <QObject>
#include <QSet>

class AbstractPlayListFilter;
class Navigator;

class RankFilterMenuManager : public QObject
{
    Q_OBJECT
public:
    explicit RankFilterMenuManager(Navigator **navigator, QObject *parent = 0);
    ~RankFilterMenuManager();

    QMenu *menu();

private slots:
    void selectionChanged(bool selected);

private:
    Navigator **m_navigator;
    QSet<int> m_unselectedRanks;
    QMenu *m_menu;
};

#endif // RANKINGFILTERMENUMANAGER_H
