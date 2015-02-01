#ifndef PLAYLISTGALLERYVIEW_H
#define PLAYLISTGALLERYVIEW_H

#include "GalleryView.h"
#include "PlayListRecord.h"

class PlayListGalleryView : public GalleryView
{
    Q_OBJECT
public:
    explicit PlayListGalleryView(QWidget *parent = 0);

signals:
    void promptToCreatePlayListRecord(int type);

public slots:
    void setPlayListRecords(QList<PlayListRecord *> records);

private slots:
    void renameSelectedItem();
    void removeSelectedItems();

    void groupByType();
    void groupByName();

protected:
    virtual void contextMenuEvent(QContextMenuEvent *event);

    QString groupForItem(GalleryItem *);
    void sortItemByGroup(QList<GalleryItem *> *items);

private:
    bool (*m_groupLessThan)(GalleryItem *, GalleryItem *);
};

#endif // PLAYLISTGALLERYVIEW_H
