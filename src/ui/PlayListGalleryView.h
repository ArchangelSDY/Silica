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
    void setPlayListRecords(QList<PlayListRecord> records);
};

#endif // PLAYLISTGALLERYVIEW_H
