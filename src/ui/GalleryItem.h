#ifndef GALLERYITEM_H
#define GALLERYITEM_H

#include <QGraphicsItem>

#include "Image.h"

class GalleryItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    explicit GalleryItem(Image *image, QGraphicsItem *parent = 0);

    virtual QRectF boundingRect() const;
    Image* image() const { return m_image; }

public slots:
    void thumbnailLoaded();
    void selectionChange(int index);

protected:
    virtual void paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *, QWidget *);

private:
    Image *m_image;
    QSize m_imageSize;
    QRect m_borderRect;
    QPoint m_imagePos;
};

#endif // GALLERYITEM_H
