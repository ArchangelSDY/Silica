#ifndef PLAYLISTGALLERYITEM_H
#define PLAYLISTGALLERYITEM_H

#include <QGraphicsItem>

#include "AbstractGalleryItemRenderer.h"
#include "GalleryItem.h"
#include "PlayListRecord.h"

class PlayListGalleryItem : public QObject, public GalleryItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    explicit PlayListGalleryItem(PlayListRecord *record,
                                 AbstractRendererFactory *rendererFactory,
                                 QGraphicsItem *parent = 0);
    ~PlayListGalleryItem();

    QRectF boundingRect() const;
    PlayListRecord *record() { return m_record; }

    void setRendererFactory(AbstractRendererFactory *factory);

    QString group() const;

private slots:
    void loadThumbnail();

protected:
    virtual void paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *, QWidget *);

private:
    PlayListRecord *m_record;
};

#endif // PLAYLISTGALLERYITEM_H
