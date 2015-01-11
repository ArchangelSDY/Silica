#ifndef COMPACTCORNERICONRENDERER_H
#define COMPACTCORNERICONRENDERER_H

#include "AbstractGalleryItemRenderer.h"

class CompactCornerIconRenderer : public AbstractGalleryItemRenderer
{
public:
    CompactCornerIconRenderer(const QImage &icon,
                              AbstractGalleryItemRenderer *parentRenderer);

    virtual void paint(QPainter *painter);

private:
    static const int CORNER_WIDTH;
    static const char *CORNER_BRUSH;

    QImage m_icon;
};

#endif // COMPACTCORNERICONRENDERER_H
