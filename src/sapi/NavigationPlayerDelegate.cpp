#include "NavigationPlayerDelegate.h"

namespace sapi {

NavigationPlayerDelegate::NavigationPlayerDelegate(INavigationPlayerPlugin *player,
                                                   Navigator *navigator,
                                                   QWidget *view) :
    AbstractNavigationPlayer(navigator) ,
    m_player(player) ,
    m_navigatorDelegate(new sapi::NavigatorDelegate(navigator))
{
    m_player->onInit(m_navigatorDelegate.data(), view);
}

QString NavigationPlayerDelegate::name() const
{
    return m_player->name();
}

void NavigationPlayerDelegate::goNext()
{
    m_player->goNext();
}

void NavigationPlayerDelegate::goPrev()
{
    m_player->goPrev();
}

void NavigationPlayerDelegate::onEnter()
{
    m_player->onEnter();
}

void NavigationPlayerDelegate::onLeave()
{
    m_player->onLeave();
}

void NavigationPlayerDelegate::reset()
{
    m_player->reset();
}

}