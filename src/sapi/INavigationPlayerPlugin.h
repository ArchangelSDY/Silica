#pragma once

#include "sapi/INavigationPlayer.h"
#include "sapi/NavigatorResource.h"
#include "sapi/sapi_global.h"

class QWidget;

namespace sapi {

class SAPI_EXPORT INavigationPlayerPlugin
{
public:
    virtual ~INavigationPlayerPlugin();

    virtual INavigationPlayer *createPlayer() = 0;
    virtual void destroyPlayer(INavigationPlayer *player) = 0;
};

}

Q_DECLARE_INTERFACE(sapi::INavigationPlayerPlugin, "com.archangelsdy.silica.plugins.navigationplayer")
