#pragma once

#include "playlist/PlayListEntityLoadContext.h"

namespace sapi {

class IPlayListEntityLoadContext;

class PlayListEntityLoadContextDelegate : public ::PlayListEntityLoadContext
{
public:
    PlayListEntityLoadContextDelegate(IPlayListEntityLoadContext *ctx);

    virtual void destroy() override;

    IPlayListEntityLoadContext *context() const;

private:
    IPlayListEntityLoadContext *m_ctx;
};

}
