#ifndef GALLERYITEM_H
#define GALLERYITEM_H

#include <QGraphicsItem>

#include "AbstractGalleryItemRenderer.h"
#include "AbstractRendererFactory.h"

class GalleryItem : public QGraphicsItem
{
public:
    explicit GalleryItem(AbstractRendererFactory *rendererFactory,
                         QGraphicsItem *parent = 0);
    virtual ~GalleryItem();

    virtual void setRendererFactory(AbstractRendererFactory *factory) = 0;

protected:
    void setRenderer(AbstractGalleryItemRenderer *renderer);

    AbstractRendererFactory *m_rendererFactory;
    AbstractGalleryItemRenderer *m_renderer;
    QImage *m_thumbnail;
};

#endif // GALLERYITEM_H
