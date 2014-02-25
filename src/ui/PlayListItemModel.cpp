#include "PlayListItemModel.h"

PlayListItemModel::PlayListItemModel(PlayListRecord *record, QObject *parent) :
    AbstractGalleryItemModel(parent) ,
    m_record(record) ,
    m_thumbnail(0)
{
}

PlayListItemModel::~PlayListItemModel()
{
    if (m_thumbnail) {
        delete m_thumbnail;
    }
}

void PlayListItemModel::loadThumbnail()
{
    if (!m_thumbnail) {
        m_thumbnail = new QImage(m_record->coverPath());
        emit thumbnailLoaded();
    }
}

QImage PlayListItemModel::thumbnail() const
{
    if (m_thumbnail) {
        return *m_thumbnail;
    } else {
        return QImage();
    }
}
