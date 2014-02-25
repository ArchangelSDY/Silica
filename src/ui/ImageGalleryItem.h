#ifndef IMAGEGALLERYITEM_H
#define IMAGEGALLERYITEM_H

#include <QGraphicsItem>

#include "Image.h"

class ImageGalleryItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
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
    QSize m_thumbnailSize;
    QPoint m_thumbnailPos;
    QRect m_borderRect;
};

#endif // IMAGEGALLERYITEM_H
