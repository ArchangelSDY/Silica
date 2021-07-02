#pragma once

class PlayListEntityLoadContext
{
public:
    virtual ~PlayListEntityLoadContext() {}
    virtual void destroy() = 0;
};

struct ScopedPointerPlayListEntityLoadContextDeleter
{
    static inline void cleanup(PlayListEntityLoadContext* ctx)
    {
        if (ctx) {
            ctx->destroy();
        }
    }
};
