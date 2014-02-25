#ifndef PLAYLISTGALLERYITEM_H
#define PLAYLISTGALLERYITEM_H

#include "AbstractGalleryItem.h"
#include "PlayListRecord.h"

class PlayListGalleryItem : public AbstractGalleryItem
{
    Q_OBJECT
public:
    explicit PlayListGalleryItem(PlayListRecord *record, QGraphicsItem *parent = 0);
    ~PlayListGalleryItem();

    QRectF boundingRect() const;

protected:
    virtual void paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *, QWidget *);

private:
    void loadThumbnail();

    PlayListRecord *m_record;
    QImage *m_image;
    QRect m_titleRect;
};

#endif // PLAYLISTGALLERYITEM_H
