#ifndef ABSTRACTGALLERYITEMRENDERER_H
#define ABSTRACTGALLERYITEMRENDERER_H

#include <QPainter>

class AbstractGalleryItemRenderer
{
public:
    AbstractGalleryItemRenderer(AbstractGalleryItemRenderer *parentRenderer = 0);

    virtual QRectF boundingRect() const { return m_boundingRect; }
    virtual void layout() = 0;
    virtual void paint(QPainter *painter) = 0;

protected:
    AbstractGalleryItemRenderer *m_parentRenderer;
    QRectF m_boundingRect;
};

#endif // ABSTRACTGALLERYITEMRENDERER_H
