#pragma once

#include "sapi_global.h"

namespace sapi {

class SAPI_EXPORT IPlayListEntityLoadContext
{
public:
    virtual ~IPlayListEntityLoadContext() {}
    virtual void destroy() = 0;
};

}
