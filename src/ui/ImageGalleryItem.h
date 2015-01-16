#ifndef IMAGEGALLERYITEM_H
#define IMAGEGALLERYITEM_H

#include <QGraphicsItem>

#include "AbstractGalleryItemRenderer.h"
#include "GalleryItem.h"
#include "Image.h"

class ImageGalleryItem : public GalleryItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    explicit ImageGalleryItem(ImagePtr image,
                              AbstractRendererFactory *rendererFactory,
                              QGraphicsItem *parent = 0);

    QString name() const;
    QRectF boundingRect() const;
    ImagePtr image() { return m_image; }

    void load();
    virtual void createRenderer();

private slots:
    void thumbnailLoaded();

private:
    ImagePtr m_image;
};

#endif // IMAGEGALLERYITEM_H
