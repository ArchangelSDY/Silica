#pragma once

class PlayListEntityLoadContext
{
public:
    virtual ~PlayListEntityLoadContext() {}
    virtual void destroy() = 0;

    static inline void deleter(PlayListEntityLoadContext* ctx)
    {
        if (ctx) {
            ctx->destroy();
        }
    }
};
