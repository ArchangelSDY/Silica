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

    /**
     * @brief Call by GalleryView during layout to check if thumbnail is ready.
     * @return True if this item has thumbnail loaded and ready to show.
     */
    virtual bool isReadyToShow();

protected:
    void setRenderer(AbstractGalleryItemRenderer *renderer);

    AbstractRendererFactory *m_rendererFactory;
    AbstractGalleryItemRenderer *m_renderer;
    QImage *m_thumbnail;
    bool m_isReadyToShow;
};

#endif // GALLERYITEM_H
