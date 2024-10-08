#ifndef LOOSEIMAGEBACKGROUNDRENDERER_H
#define LOOSEIMAGEBACKGROUNDRENDERER_H

#include "AbstractGalleryItemRenderer.h"

class LooseImageBackgroundRenderer : public AbstractGalleryItemRenderer
{
public:
    LooseImageBackgroundRenderer(AbstractGalleryItemRenderer *parentRenderer);

    virtual void layout(const QRect &viewGeometry) override;
    virtual void paint(QPainter *painter) override;

private:
    static const int ANGLE = 3;
    static const int RADIUS = 5;
};

#endif // LOOSEIMAGEBACKGROUNDRENDERER_H
