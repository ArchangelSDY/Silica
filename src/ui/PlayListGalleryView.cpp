#include "PlayListGalleryItem.h"
#include "PlayListGalleryView.h"

PlayListGalleryView::PlayListGalleryView(QWidget *parent) :
    GalleryView(parent)
{
}

void PlayListGalleryView::setPlayListRecords(QList<PlayListRecord> records)
{
    clear();

    for (int i = 0; i < records.count(); ++i) {
        PlayListGalleryItem *item = new PlayListGalleryItem(
            new PlayListRecord(records[i]));
        m_scene->addItem(item);
    }

    layout();
}
