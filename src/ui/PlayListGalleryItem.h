#ifndef PLAYLISTGALLERYITEM_H
#define PLAYLISTGALLERYITEM_H

#include <QGraphicsItem>

#include "AbstractGalleryItemRenderer.h"
#include "GalleryItem.h"
#include "PlayListRecord.h"

class PlayListGalleryItem : public GalleryItem
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

    void load();
    virtual void createRenderer();

private slots:
    void loadThumbnail();

private:
    PlayListRecord *m_record;
};

#endif // PLAYLISTGALLERYITEM_H
