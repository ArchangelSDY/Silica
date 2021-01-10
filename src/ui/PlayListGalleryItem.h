#ifndef PLAYLISTGALLERYITEM_H
#define PLAYLISTGALLERYITEM_H

#include <QFutureWatcher>
#include <QGraphicsItem>

#include "playlist/PlayListEntity.h"
#include "AbstractGalleryItemRenderer.h"
#include "GalleryItem.h"

class PlayListGalleryItem : public GalleryItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    explicit PlayListGalleryItem(QSharedPointer<PlayListEntity> entity,
                                 AbstractRendererFactory *rendererFactory,
                                 QGraphicsItem *parent = 0);
    ~PlayListGalleryItem();

    QString name() const;
    QRectF boundingRect() const;
    QSharedPointer<PlayListEntity> entity() { return m_entity; }

    virtual void load() override;
    virtual void createRenderer();

private slots:
    void loadThumbnail();
    void onThumbnailLoaded();

private:
    QSharedPointer<PlayListEntity> m_entity;
    QFutureWatcher<QSharedPointer<QImage>> m_thumbnailLoader;
};

#endif // PLAYLISTGALLERYITEM_H
