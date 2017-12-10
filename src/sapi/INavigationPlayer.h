#pragma once

#include "sapi_global.h"
#include "NavigatorResource.h"

namespace sapi {

class SAPI_EXPORT INavigationPlayer
{
public:
    virtual ~INavigationPlayer();

    virtual QString name() const = 0;

    virtual void goNext() = 0;
    virtual void goPrev() = 0;

    virtual void onInit(NavigatorResource *nav, QWidget *view) = 0;
    virtual void onEnter() = 0;
    virtual void onLeave() = 0;
    virtual void reset() = 0;
};

}
