#pragma once

#include <QList>

#include "AbstractNavigationPlayer.h"

class QWidget;
class Navigator;

class NavigationPlayerManager
{
public:
    ~NavigationPlayerManager();

    AbstractNavigationPlayer *get(int idx);
    QList<AbstractNavigationPlayer *> all() const;

    void registerPlayer(AbstractNavigationPlayer *player);
    void init(Navigator *navigator, QWidget *view);

    static NavigationPlayerManager *instance();

private:
    NavigationPlayerManager();

    static NavigationPlayerManager *s_instance;
    QList<AbstractNavigationPlayer *> m_players;
};