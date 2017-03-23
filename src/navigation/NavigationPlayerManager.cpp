#include "NavigationPlayerManager.h"

#include "CascadeClassifierNavigationPlayer.h"
#include "ExpandingNavigationPlayer.h"
#include "FixedRegionNavigationPlayer.h"
#include "HotspotsNavigationPlayer.h"
#include "NormalNavigationPlayer.h"

NavigationPlayerManager *NavigationPlayerManager::s_instance = nullptr;

NavigationPlayerManager *NavigationPlayerManager::instance()
{
    if (!s_instance) {
        s_instance = new NavigationPlayerManager();
    }
    return s_instance;
}

NavigationPlayerManager::NavigationPlayerManager()
{

}

NavigationPlayerManager::~NavigationPlayerManager()
{
    for (AbstractNavigationPlayer *player : m_players) {
        delete player;
    }
}

AbstractNavigationPlayer *NavigationPlayerManager::get(int idx)
{
    if (idx < 0 || idx >= m_players.size()) {
        return nullptr;
    }
    return m_players[idx];
}

QList<AbstractNavigationPlayer *> NavigationPlayerManager::all() const
{
    return m_players;
}

void NavigationPlayerManager::registerPlayer(AbstractNavigationPlayer *player)
{
    m_players << player;
}

void NavigationPlayerManager::init(Navigator *navigator, QWidget *view)
{
    registerPlayer(new NormalNavigationPlayer(navigator));
    registerPlayer(new HotspotsNavigationPlayer(navigator));
    registerPlayer(new ExpandingNavigationPlayer(navigator, view));
    registerPlayer(new FixedRegionNavigationPlayer(navigator, view));
    registerPlayer(new CascadeClassifierNavigationPlayer(navigator, view));
}