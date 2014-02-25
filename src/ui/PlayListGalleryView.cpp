#include "GalleryItem.h"
#include "PlayListItemModel.h"
#include "PlayListGalleryView.h"

PlayListGalleryView::PlayListGalleryView(QWidget *parent) :
    GalleryView(parent)
{
}

void PlayListGalleryView::setPlayListRecords(QList<PlayListRecord> records)
{
    clear();

    for (int i = 0; i < records.count(); ++i) {
        PlayListItemModel *model = new PlayListItemModel(&records[i]);
        GalleryItem *item = new GalleryItem(model);
        m_scene->addItem(item);
    }

    layout();
}
