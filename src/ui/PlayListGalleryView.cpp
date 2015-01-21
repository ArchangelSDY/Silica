#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>

#include "CompactRendererFactory.h"
#include "LooseRendererFactory.h"
#include "PlayListGalleryItem.h"
#include "PlayListGalleryView.h"

static bool playListTypeLessThan(GalleryItem *left,
                                 GalleryItem *right)

{
    PlayListGalleryItem *leftItem = static_cast<PlayListGalleryItem *>(left);
    PlayListGalleryItem *rightItem = static_cast<PlayListGalleryItem *>(right);

    int leftRecordType = leftItem->record()->type();
    int rightRecordType = rightItem->record()->type();

    if (leftRecordType < rightRecordType) {
        return true;
    } else if (leftRecordType > rightRecordType) {
        return false;
    } else {
        return leftItem->record()->name() < rightItem->record()->name();
    }
}

static bool playListNameLessThan(GalleryItem *left,
                                 GalleryItem *right)

{
    PlayListGalleryItem *leftItem = static_cast<PlayListGalleryItem *>(left);
    PlayListGalleryItem *rightItem = static_cast<PlayListGalleryItem *>(right);

    const QString &leftName = leftItem->record()->name();
    const QString &rightName = rightItem->record()->name();

    return leftName < rightName;
}

PlayListGalleryView::PlayListGalleryView(QWidget *parent) :
    GalleryView(parent) ,
    m_groupLessThan(playListTypeLessThan)
{
    setRendererFactory(new CompactRendererFactory());

    // Default
    groupByType();
}

void PlayListGalleryView::setPlayListRecords(QList<PlayListRecord *> records)
{
    clear();

    incrItemsToLoad(records.count());

    for (int i = 0; i < records.count(); ++i) {
        PlayListGalleryItem *item = new PlayListGalleryItem(records[i],
            rendererFactory());
        addItem(item);
    }

    scheduleLayout();
}

void PlayListGalleryView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    QList<GalleryItem *> selectedItems = selectedGalleryItems();

    QMenu *menuNew = menu.addMenu(tr("New"));
    menuNew->addAction(tr("Remote PlayList"), this,
        SIGNAL(promptToSaveRemotePlayList()));

    QAction *actRename =
        menu.addAction(tr("Rename"), this, SLOT(renameSelectedItem()));
    if (selectedItems.count() == 0) {
        actRename->setEnabled(false);
    }

    QAction *actRemove =
        menu.addAction(tr("Remove"), this, SLOT(removeSelectedItems()));
    if (selectedItems.count() == 0) {
        actRemove->setEnabled(false);
    }

    menu.addSeparator();

    QMenu *renderers = menu.addMenu(tr("Layout"));
    renderers->addAction(tr("Loose"), this, SLOT(setLooseRenderer()));
    renderers->addAction(tr("Compact"), this, SLOT(setCompactRenderer()));

    QMenu *groups = menu.addMenu(tr("Group By"));
    QAction *actDisableGrouping =
        groups->addAction(tr("None"), this, SLOT(disableGrouping()));
    actDisableGrouping->setCheckable(true);
    actDisableGrouping->setChecked(!m_enableGrouping);
    QAction *actGroupByType =
        groups->addAction(tr("Type"), this, SLOT(groupByType()));
    actGroupByType->setCheckable(true);
    actGroupByType->setChecked(
        m_enableGrouping && m_groupLessThan == playListTypeLessThan);
    QAction *actGroupByName =
        groups->addAction(tr("Name"), this, SLOT(groupByName()));
    actGroupByName->setCheckable(true);
    actGroupByName->setChecked(
        m_enableGrouping && m_groupLessThan == playListNameLessThan);

    menu.exec(event->globalPos());
}

void PlayListGalleryView::renameSelectedItem()
{
    QList<GalleryItem *> selectedItems = selectedGalleryItems();
    if (selectedItems.count() > 0) {
        PlayListGalleryItem *item =
            static_cast<PlayListGalleryItem *>(selectedItems[0]);
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
    QList<GalleryItem *> selectedItems = selectedGalleryItems();
    if (selectedItems.count() > 0) {
        // Build message with playlists names
        QStringList playListNames;
        foreach(GalleryItem *item, selectedItems) {
            PlayListGalleryItem *playListItem =
                static_cast<PlayListGalleryItem *>(item);
            playListNames.append(playListItem->record()->name());
        }
        QString msg = tr("Remove %1 playlist: \n%2 ?")
            .arg(playListNames.count())
            .arg(playListNames.join("\n"));

        if (QMessageBox::question(
                this, tr("Remove"), msg) == QMessageBox::Yes) {
            foreach (QGraphicsItem *item, selectedItems) {
                PlayListGalleryItem *playListItem =
                    static_cast<PlayListGalleryItem *>(item);
                playListItem->record()->remove();
                scene()->removeItem(item);
            }

            scheduleLayout();
        }
    }
}

QString PlayListGalleryView::groupForItem(GalleryItem *rawItem)
{
    PlayListGalleryItem *item = static_cast<PlayListGalleryItem *>(rawItem);
    if (m_groupLessThan == playListTypeLessThan) {
        return item->record()->typeName();
    } else if (m_groupLessThan == playListNameLessThan) {
        return item->record()->name().left(5);
    } else {
        return QString();
    }
}

void PlayListGalleryView::sortItemByGroup(QList<GalleryItem *> *items)
{
    qSort(items->begin(), items->end(), m_groupLessThan);
}

void PlayListGalleryView::groupByType()
{
    m_groupLessThan = playListTypeLessThan;
    enableGrouping();
}

void PlayListGalleryView::groupByName()
{
    m_groupLessThan = playListNameLessThan;
    enableGrouping();
}
