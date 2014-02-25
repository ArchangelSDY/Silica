#ifndef PLAYLISTITEMMODEL_H
#define PLAYLISTITEMMODEL_H

#include "AbstractGalleryItemModel.h"
#include "PlayListRecord.h"

class PlayListItemModel : public AbstractGalleryItemModel
{
public:
    explicit PlayListItemModel(PlayListRecord *record, QObject *parent = 0);
    ~PlayListItemModel();

    void loadThumbnail();
    QImage thumbnail() const;

private:
    PlayListRecord *m_record;
    QImage *m_thumbnail;
};

#endif // PLAYLISTITEMMODEL_H
