#pragma once

#include "sapi/NavigatorResource.h"
#include "sapi/sapi_global.h"

class QWidget;

namespace sapi {

class SAPI_EXPORT INavigationPlayerPlugin
{
public:
    virtual ~INavigationPlayerPlugin();

    virtual QString name() const = 0;

    virtual void goNext() = 0;
    virtual void goPrev() = 0;

    virtual void onInit(NavigatorResource *nav, QWidget *view) = 0;
    virtual void onEnter() = 0;
    virtual void onLeave() = 0;
    virtual void reset() = 0;
};

}

Q_DECLARE_INTERFACE(sapi::INavigationPlayerPlugin, "com.archangelsdy.silica.plugins.navigationplayer")