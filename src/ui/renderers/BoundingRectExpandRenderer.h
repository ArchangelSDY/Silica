#ifndef BOUNDINGRECTEXPANDRENDERER_H
#define BOUNDINGRECTEXPANDRENDERER_H

#include "ui/renderers/AbstractGalleryItemRenderer.h"

class BoundingRectExpandRenderer : public AbstractGalleryItemRenderer
{
public:
    BoundingRectExpandRenderer(const QRect &expandedRect,
                               AbstractGalleryItemRenderer *parentRenderer);
};

#endif // BOUNDINGRECTEXPANDRENDERER_H
