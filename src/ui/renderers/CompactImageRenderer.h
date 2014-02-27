#ifndef COMPACTIMAGERENDERER_H
#define COMPACTIMAGERENDERER_H

#include "AbstractGalleryItemRenderer.h"
#include "Image.h"

class CompactImageRenderer : public AbstractGalleryItemRenderer
{
public:
    CompactImageRenderer();

    virtual void layout();
    virtual void paint(QPainter *painter);

private:
    static const int BORDER;

    QRect m_imageRect;
    QRect m_imageSourceRect;
};

#endif // COMPACTIMAGERENDERER_H
