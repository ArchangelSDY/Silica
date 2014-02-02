#ifndef GALLERYITEM_H
#define GALLERYITEM_H

#include <QGraphicsPixmapItem>

#include "../Image.h"

class GalleryItem : public QGraphicsPixmapItem
{
public:
    explicit GalleryItem(Image *image, QGraphicsItem *parent = 0);

private:
    Image *m_image;

};

#endif // GALLERYITEM_H
