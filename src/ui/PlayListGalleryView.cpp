#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>

#include "CompactRendererFactory.h"
#include "LooseRendererFactory.h"
#include "PlayListGalleryItem.h"
#include "PlayListGalleryView.h"

PlayListGalleryView::PlayListGalleryView(QWidget *parent) :
    GalleryView(parent)
{
    m_rendererFactory = new CompactRendererFactory();
    m_enableGrouping = true;
}

void PlayListGalleryView::setPlayListRecords(QList<PlayListRecord *> records)
{
    clear();

    for (int i = 0; i < records.count(); ++i) {
        PlayListGalleryItem *item = new PlayListGalleryItem(records[i],
            m_rendererFactory);
        m_scene->addItem(item);
    }

    layout();
}

void PlayListGalleryView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);

    QMenu *menuNew = menu.addMenu(tr("New"));
    menuNew->addAction(tr("Remote PlayList"), this,
        SIGNAL(promptToSaveRemotePlayList()));

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

    menu.addSeparator();

    QMenu *renderers = menu.addMenu(tr("Layout"));
    renderers->addAction(tr("Loose"), this, SLOT(setLooseRenderer()));
    renderers->addAction(tr("Compact"), this, SLOT(setCompactRenderer()));

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
        if (QMessageBox::question(this,
            tr("Remove"), tr("Remove this playlist?")) == QMessageBox::Yes) {
            foreach (QGraphicsItem *item, scene()->selectedItems()) {
                PlayListGalleryItem *playListItem =
                    static_cast<PlayListGalleryItem *>(item);
                playListItem->record()->remove();
                scene()->removeItem(item);
            }

            layout();
        }
    }
}
