#ifndef LOOSEIMAGERENDERER_H
#define LOOSEIMAGERENDERER_H

#include "AbstractGalleryItemRenderer.h"
#include "Image.h"

class LooseImageRenderer : public AbstractGalleryItemRenderer
{
public:
    LooseImageRenderer();

    virtual Qt::AspectRatioMode aspectRatioMode() const;
    virtual void layout();
    virtual void paint(QPainter *painter);

private:
    static const int PADDING;
    static const int BORDER;

    QRect m_imageRect;
    QRect m_borderRect;
};

#endif // LOOSEIMAGERENDERER_H
