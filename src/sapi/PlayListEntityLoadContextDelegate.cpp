#include "sapi/PlayListEntityLoadContextDelegate.h"

#include "sapi/IPlayListEntityLoadContext.h"

namespace sapi {

PlayListEntityLoadContextDelegate::PlayListEntityLoadContextDelegate(sapi::IPlayListEntityLoadContext *ctx) :
    m_ctx(ctx)
{
}

void PlayListEntityLoadContextDelegate::destroy()
{
   m_ctx->destroy();
}

IPlayListEntityLoadContext *PlayListEntityLoadContextDelegate::context() const
{
    return m_ctx;
}

}

