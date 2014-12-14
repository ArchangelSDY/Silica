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
    void promptToSaveRemotePlayList();

public slots:
    void setPlayListRecords(QList<PlayListRecord *> records);

private slots:
    void renameSelectedItem();
    void removeSelectedItems();

    void groupByType();
    void groupByName();

protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void contextMenuEvent(QContextMenuEvent *event);

    QString groupForItem(QGraphicsItem *);
    void sortItemByGroup(QList<QGraphicsItem *> *items);

private:
    bool (*m_groupLessThan)(QGraphicsItem *, QGraphicsItem *);
};

#endif // PLAYLISTGALLERYVIEW_H
