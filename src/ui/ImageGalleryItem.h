#ifndef IMAGEGALLERYITEM_H
#define IMAGEGALLERYITEM_H

#include <QGraphicsItem>

#include "AbstractGalleryItemRenderer.h"
#include "GalleryItem.h"
#include "Image.h"

class ImageGalleryItem : public QObject, public GalleryItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    explicit ImageGalleryItem(ImagePtr image,
                              AbstractRendererFactory *rendererFactory,
                              QGraphicsItem *parent = 0);

    QRectF boundingRect() const;
    ImagePtr image() { return m_image; }

    void setRendererFactory(AbstractRendererFactory *factory);

private slots:
    void thumbnailLoaded();

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget);

private:
    ImagePtr m_image;
};

#endif // IMAGEGALLERYITEM_H
