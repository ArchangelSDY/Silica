#ifndef PLAYLISTGALLERYVIEW_H
#define PLAYLISTGALLERYVIEW_H

#include "GalleryView.h"
#include "PlayListRecord.h"

class PlayListGalleryView : public GalleryView
{
    Q_OBJECT
public:
    explicit PlayListGalleryView(QWidget *parent = 0);

public slots:
    void setPlayListRecords(QList<PlayListRecord *> records);

private slots:
    void renameSelectedItem();
    void removeSelectedItems();

protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void contextMenuEvent(QContextMenuEvent *event);
};

#endif // PLAYLISTGALLERYVIEW_H
