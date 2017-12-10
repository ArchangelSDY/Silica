#pragma once

#include <QScopedPointer>

#include "sapi/INavigationPlayer.h"
#include "sapi/INavigationPlayerPlugin.h"
#include "sapi/NavigatorDelegate.h"
#include "navigation/AbstractNavigationPlayer.h"

namespace sapi {

class NavigationPlayerDelegate : public AbstractNavigationPlayer
{
public:
    NavigationPlayerDelegate(INavigationPlayerPlugin *plugin,
                             Navigator *navigator,
                             QWidget *view);
    ~NavigationPlayerDelegate();

    virtual QString name() const override;
    virtual void goNext() override;
    virtual void goPrev() override;
    virtual void onEnter() override;
    virtual void onLeave() override;
    virtual void reset() override;

private:
    INavigationPlayerPlugin *m_plugin;
    INavigationPlayer *m_player;
    QScopedPointer<NavigatorResource> m_navigatorDelegate;
};

}
