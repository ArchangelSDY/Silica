#include <QMenu>

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

void PlayListGalleryView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    QAction *actRemove =
        menu.addAction(tr("Remove"), this, SLOT(removeSelectedItems()));

    if (scene()->selectedItems().count() == 0) {
        actRemove->setEnabled(false);
    }

    menu.exec(event->globalPos());
}

void PlayListGalleryView::mousePressEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::RightButton) {
        ev->accept();
        return;
    }

    QGraphicsView::mousePressEvent(ev);
}

void PlayListGalleryView::removeSelectedItems()
{
    if (scene()->selectedItems().count() > 0) {
        foreach (QGraphicsItem *item, scene()->selectedItems()) {
            PlayListGalleryItem *playListItem =
                static_cast<PlayListGalleryItem *>(item);
            playListItem->record()->remove();
            scene()->removeItem(item);
        }

        layout();
    }
}
