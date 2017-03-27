#pragma once

#include <QScopedPointer>

#include "sapi/INavigationPlayerPlugin.h"
#include "sapi/NavigatorDelegate.h"
#include "navigation/AbstractNavigationPlayer.h"

namespace sapi {

class NavigationPlayerDelegate : public AbstractNavigationPlayer
{
public:
    NavigationPlayerDelegate(INavigationPlayerPlugin *player,
                             Navigator *navigator,
                             QWidget *view);

    virtual QString name() const override;
    virtual void goNext() override;
    virtual void goPrev() override;
    virtual void onEnter() override;
    virtual void onLeave() override;
    virtual void reset() override;

private:
    QScopedPointer<INavigationPlayerPlugin> m_player;
    QScopedPointer<NavigatorResource> m_navigatorDelegate;
};

}
