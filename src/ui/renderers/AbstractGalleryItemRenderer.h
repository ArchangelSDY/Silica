#ifndef ABSTRACTGALLERYITEMRENDERER_H
#define ABSTRACTGALLERYITEMRENDERER_H

#include <QPainter>

class AbstractGalleryItemRenderer
{
public:
    AbstractGalleryItemRenderer(AbstractGalleryItemRenderer *parentRenderer = 0);
    virtual ~AbstractGalleryItemRenderer();

    void setImage(const QImage *image);

    virtual QRect boundingRect() const { return m_boundingRect; }
    virtual void layout();
    virtual void paint(QPainter *painter);

protected:
    AbstractGalleryItemRenderer *m_parentRenderer;
    QRect m_boundingRect;
    const QImage *m_image;
};

#endif // ABSTRACTGALLERYITEMRENDERER_H
