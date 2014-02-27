#ifndef PLAYLISTGALLERYITEM_H
#define PLAYLISTGALLERYITEM_H

#include <QGraphicsItem>

#include "AbstractGalleryItemRenderer.h"
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
    PlayListRecord *record() { return m_record; }

protected:
    virtual void paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *, QWidget *);

private:
    void loadThumbnail();

    PlayListRecord *m_record;
    QImage *m_image;
    AbstractGalleryItemRenderer *m_renderer;
};

#endif // PLAYLISTGALLERYITEM_H
