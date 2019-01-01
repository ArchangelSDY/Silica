#pragma once

#include <QScopedPointer>

#include "sapi/INavigationPlayer.h"
#include "sapi/INavigationPlayerPlugin.h"
#include "sapi/NavigatorDelegate.h"
#include "navigation/AbstractNavigationPlayer.h"

namespace sapi {

class NavigationPlayerDelegate : public AbstractNavigationPlayer
{
    Q_OBJECT
public:
    NavigationPlayerDelegate(INavigationPlayerPlugin *plugin,
                             const QString &className,
                             Navigator *navigator,
                             QWidget *view);
    ~NavigationPlayerDelegate();

    virtual QString name() const override;
    virtual QString className() const override;
    virtual void goNext() override;
    virtual void goPrev() override;
    virtual void onEnter() override;
    virtual void onLeave() override;
    virtual void reset() override;

private:
    INavigationPlayerPlugin *m_plugin;
    INavigationPlayer *m_player;
    QString m_className;
    QScopedPointer<NavigatorResource> m_navigatorDelegate;
};

}
