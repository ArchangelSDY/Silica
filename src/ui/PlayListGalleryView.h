#ifndef PLAYLISTGALLERYVIEW_H
#define PLAYLISTGALLERYVIEW_H

#include "playlist/PlayListEntity.h"
#include "GalleryView.h"

class PlayListGalleryView : public GalleryView
{
    Q_OBJECT
public:
    explicit PlayListGalleryView(QWidget *parent = 0);

signals:
    void promptToCreatePlayListRecord(int type);

public slots:
    void setPlayListEntities(QList<PlayListEntity *> entities);

private slots:
    void renameSelectedItem();
    void removeSelectedItems();

    void groupByType();
    void groupByName();

protected:
    virtual void contextMenuEvent(QContextMenuEvent *event);

    virtual QString groupForItem(GalleryItem *item) override;
    void sortItemByGroup(QList<GalleryItem *> *items);

private:
    bool (*m_groupLessThan)(GalleryItem *, GalleryItem *);
};

#endif // PLAYLISTGALLERYVIEW_H
