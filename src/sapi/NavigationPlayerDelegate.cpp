#include "NavigationPlayerDelegate.h"

namespace sapi {

NavigationPlayerDelegate::NavigationPlayerDelegate(INavigationPlayerPlugin *plugin,
                                                   Navigator *navigator,
                                                   QWidget *view) :
    AbstractNavigationPlayer(navigator) ,
    m_plugin(plugin) ,
    m_player(plugin->createPlayer()) ,
    m_navigatorDelegate(new sapi::NavigatorDelegate(navigator))
{
    m_player->onInit(m_navigatorDelegate.data(), view);
}

NavigationPlayerDelegate::~NavigationPlayerDelegate()
{
    m_plugin->destroyPlayer(m_player);
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