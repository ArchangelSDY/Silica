#include <QInputDialog>
#include <QMenu>

#include "PlayListGalleryItem.h"
#include "PlayListGalleryView.h"

PlayListGalleryView::PlayListGalleryView(QWidget *parent) :
    GalleryView(parent)
{
}

void PlayListGalleryView::setPlayListRecords(QList<PlayListRecord *> records)
{
    clear();

    for (int i = 0; i < records.count(); ++i) {
        PlayListGalleryItem *item = new PlayListGalleryItem(records[i]);
        m_scene->addItem(item);
    }

    layout();
}

void PlayListGalleryView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);

    QAction *actRename =
        menu.addAction(tr("Rename"), this, SLOT(renameSelectedItem()));
    if (scene()->selectedItems().count() == 0) {
        actRename->setEnabled(false);
    }

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

void PlayListGalleryView::renameSelectedItem()
{
    if (scene()->selectedItems().count() > 0) {
        PlayListGalleryItem *item =
            static_cast<PlayListGalleryItem *>(scene()->selectedItems()[0]);
        PlayListRecord *record = item->record();

        QString newName = QInputDialog::getText(
            0, "Rename PlayList", "New Name",
            QLineEdit::Normal, record->name());

        if (!newName.isEmpty()) {
            record->setName(newName);
            record->save();
        }
    }
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
