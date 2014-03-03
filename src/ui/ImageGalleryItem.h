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
    explicit ImageGalleryItem(Image *image,
                              AbstractRendererFactory *rendererFactory,
                              QGraphicsItem *parent = 0);

    QRectF boundingRect() const;
    Image *image() { return m_image; }

    void setRendererFactory(AbstractRendererFactory *factory);

private slots:
    void thumbnailLoaded();

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget);

private:
    Image *m_image;
};

#endif // IMAGEGALLERYITEM_H
