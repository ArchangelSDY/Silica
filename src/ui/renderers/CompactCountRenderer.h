#ifndef COMPACTCOUNTRENDERER_H
#define COMPACTCOUNTRENDERER_H

#include "AbstractGalleryItemRenderer.h"

class CompactCountRenderer : public AbstractGalleryItemRenderer
{
public:
    CompactCountRenderer(const int count,
                         AbstractGalleryItemRenderer *parentRenderer);

    virtual void paint(QPainter *painter);

private:
    static const int CORNER_WIDTH;
    static const char *CORNER_BRUSH;

    int m_count;
};

#endif // COMPACTCOUNTRENDERER_H
