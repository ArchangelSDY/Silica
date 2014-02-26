#ifndef PLAYLISTGALLERYITEM_H
#define PLAYLISTGALLERYITEM_H

#include <QGraphicsItem>

#include "PlayListRecord.h"

class PlayListGalleryItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    explicit PlayListGalleryItem(PlayListRecord *record,
                                 QGraphicsItem *parent = 0);
    ~PlayListGalleryItem();

    QRectF boundingRect() const;

protected:
    virtual void paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *, QWidget *);

private:
    void loadThumbnail();

    PlayListRecord *m_record;
    QImage *m_image;
    QSize m_thumbnailSize;
    QPoint m_thumbnailPos;
    QRect m_titleRect;
};

#endif // PLAYLISTGALLERYITEM_H
