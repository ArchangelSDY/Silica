#ifndef GALLERYITEM_H
#define GALLERYITEM_H

#include <QGraphicsPixmapItem>

#include "../Image.h"

class GalleryItem : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    explicit GalleryItem(Image *image, QGraphicsItem *parent = 0);

public slots:
    void thumbnailLoaded();

protected:
    virtual void paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *, QWidget *);

private:
    Image *m_image;
};

#endif // GALLERYITEM_H
