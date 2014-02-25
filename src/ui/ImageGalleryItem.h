#ifndef IMAGEGALLERYITEM_H
#define IMAGEGALLERYITEM_H

#include "AbstractGalleryItem.h"
#include "Image.h"

class ImageGalleryItem : public AbstractGalleryItem
{
    Q_OBJECT
public:
    explicit ImageGalleryItem(Image *image,
                              QGraphicsItem *parent = 0);

    QRectF boundingRect() const;
    Image *image() { return m_image; }

private slots:
    void thumbnailLoaded();

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget);

private:
    Image *m_image;
    QRect m_borderRect;
};

#endif // IMAGEGALLERYITEM_H
