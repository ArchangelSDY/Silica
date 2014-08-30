#ifndef WATERFALLIMAGERENDERER_H
#define WATERFALLIMAGERENDERER_H

#include "AbstractGalleryItemRenderer.h"

class WaterfallImageRenderer : public AbstractGalleryItemRenderer
{
public:
    WaterfallImageRenderer();

    virtual void layout();
    virtual void paint(QPainter *painter);

private:
    static const int PADDING;
    static const int BORDER;

    QRect m_imageRect;
    QRect m_borderRect;
};

#endif // WATERFALLIMAGERENDERER_H
