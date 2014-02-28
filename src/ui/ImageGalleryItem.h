#ifndef IMAGEGALLERYITEM_H
#define IMAGEGALLERYITEM_H

#include <QGraphicsItem>

#include "AbstractGalleryItemRenderer.h"
#include "Image.h"

class ImageGalleryItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    explicit ImageGalleryItem(Image *image,
                              QGraphicsItem *parent = 0);
    ~ImageGalleryItem();

    QRectF boundingRect() const;
    Image *image() { return m_image; }

    void setRenderer(AbstractGalleryItemRenderer *renderer);

private slots:
    void thumbnailLoaded();

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget);

private:
    Image *m_image;
    QImage *m_thumbnail;
    AbstractGalleryItemRenderer *m_renderer;
};

#endif // IMAGEGALLERYITEM_H
