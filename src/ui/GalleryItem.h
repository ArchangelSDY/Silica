#ifndef GALLERYITEM_H
#define GALLERYITEM_H

#include <QGraphicsItem>

#include "AbstractGalleryItemModel.h"

class GalleryItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    explicit GalleryItem(AbstractGalleryItemModel *model,
                         QGraphicsItem *parent = 0);

    virtual QRectF boundingRect() const;

    AbstractGalleryItemModel *model() { return m_model; }

public slots:
    void thumbnailLoaded();

protected:
    virtual void paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *, QWidget *);

private:
    AbstractGalleryItemModel *m_model;
    QSize m_thumbnailSize;
    QRect m_borderRect;
    QPoint m_thumbnailPos;
};

#endif // GALLERYITEM_H
