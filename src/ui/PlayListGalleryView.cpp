#include "PlayListGalleryView.h"

#include <QContextMenuEvent>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QSignalMapper>
#include <QtConcurrent>

#include "playlist/PlayListProvider.h"
#include "playlist/PlayListProviderManager.h"
#include "ui/renderers/CompactRendererFactory.h"
#include "ui/renderers/LooseRendererFactory.h"
#include "ui/PlayListGalleryItem.h"

static bool playListTypeLessThan(GalleryItem *left,
    GalleryItem *right)

{
    PlayListGalleryItem *leftItem = static_cast<PlayListGalleryItem *>(left);
    PlayListGalleryItem *rightItem = static_cast<PlayListGalleryItem *>(right);

    // TODO
    // int leftRecordType = leftItem->entity()->type();
    // int rightRecordType = rightItem->entity()->type();

    // if (leftRecordType < rightRecordType) {
    //     return true;
    // } else if (leftRecordType > rightRecordType) {
    //     return false;
    // } else {
    //     return leftItem->record()->name() < rightItem->record()->name();
    // }

    return leftItem->entity()->name() < rightItem->entity()->name();
}

static bool playListNameLessThan(GalleryItem *left,
    GalleryItem *right)

{
    PlayListGalleryItem *leftItem = static_cast<PlayListGalleryItem *>(left);
    PlayListGalleryItem *rightItem = static_cast<PlayListGalleryItem *>(right);

    const QString &leftName = leftItem->entity()->name();
    const QString &rightName = rightItem->entity()->name();

    return leftName < rightName;
}

PlayListGalleryView::PlayListGalleryView(QWidget *parent) :
    GalleryView(parent),
    m_groupLessThan(playListTypeLessThan)
{
    setRendererFactory(new CompactRendererFactory());

    // Default
    groupByType();
}

void PlayListGalleryView::setPlayListEntities(QList<PlayListEntity *> entities)
{
    clear();

    incrItemsToLoad(entities.count());

    for (int i = 0; i < entities.count(); ++i) {
        PlayListGalleryItem *item = new PlayListGalleryItem(entities[i],
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
    QSignalMapper *plrCreatorMap = new QSignalMapper(&menu);
    for (auto provider : PlayListProviderManager::instance()->all()) {
        if (provider->supportsOption(PlayListProviderOption::CreateEntity)) {
            QAction *act = menuNew->addAction(provider->name(), plrCreatorMap, SLOT(map()));
            plrCreatorMap->setMapping(act, provider->type());
        }
    }
    connect(plrCreatorMap, qOverload<int>(&QSignalMapper::mapped),
        this, &PlayListGalleryView::createPlayListEntity);

    if (selectedItems.count() > 0) {
        QAction *actRename = menu.addAction(tr("Rename"), this, SLOT(renameSelectedItem()));
        QAction *actRemove = menu.addAction(tr("Remove"), this, SLOT(removeSelectedItems()));
        for (GalleryItem *item : selectedItems) {
            PlayListGalleryItem *playListItem = static_cast<PlayListGalleryItem *>(item);
            auto entity = playListItem->entity();
            if (!entity->provider()->supportsOption(PlayListProviderOption::UpdateEntity)) {
                actRename->setEnabled(false);
                break;
            }
            if (!entity->provider()->supportsOption(PlayListProviderOption::RemoveEntity)) {
                actRemove->setEnabled(false);
                break;
            }
        }
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

void PlayListGalleryView::createPlayListEntity(int type)
{
    auto provider = PlayListProviderManager::instance()->get(type);
    Q_ASSERT(provider);
    Q_ASSERT(provider->supportsOption(PlayListProviderOption::CreateEntity));

    // TODO: Support custom fields in addition to name
    QString name = QInputDialog::getText(this, "New PlayList", "Name");
    if (!name.isEmpty()) {
        auto entity = provider->createEntity(name);
        QtConcurrent::run([provider, entity]() {
            provider->insertEntity(entity);
        });
    }
}

void PlayListGalleryView::renameSelectedItem()
{
    QList<GalleryItem *> selectedItems = selectedGalleryItems();
    if (selectedItems.count() > 0) {
        PlayListGalleryItem *item =
            static_cast<PlayListGalleryItem *>(selectedItems[0]);
        PlayListEntity *entity = item->entity();

        QString newName = QInputDialog::getText(
            0, "Rename PlayList", "New Name",
            QLineEdit::Normal, entity->name());

         if (!newName.isEmpty()) {
             entity->setName(newName);
             entity->provider()->updateEntity(entity);
         }
    }
}

void PlayListGalleryView::removeSelectedItems()
{
    QList<GalleryItem *> selectedItems = selectedGalleryItems();
    if (selectedItems.count() > 0) {
        // Build message with playlists names
        QStringList playListNames;
        foreach(GalleryItem * item, selectedItems) {
            PlayListGalleryItem *playListItem =
                static_cast<PlayListGalleryItem *>(item);
            playListNames.append(playListItem->entity()->name());
        }
        QString msg = tr("Remove %1 playlist: \n%2 ?")
            .arg(playListNames.count())
            .arg(playListNames.join("\n"));

        if (QMessageBox::question(
                this, tr("Remove"), msg) == QMessageBox::Yes) {
            foreach (QGraphicsItem *item, selectedItems) {
                PlayListGalleryItem *playListItem = static_cast<PlayListGalleryItem *>(item);
                auto entity = playListItem->entity();
                QtConcurrent::run([entity]() {
                    entity->provider()->removeEntity(entity);
                    delete entity;
                });
            }
        }
    }
}

QString PlayListGalleryView::groupForItem(GalleryItem *rawItem)
{
    // TODO
    // PlayListGalleryItem *item = static_cast<PlayListGalleryItem *>(rawItem);
    // if (m_groupLessThan == playListTypeLessThan) {
    //     return item->record()->typeName();
    // } else if (m_groupLessThan == playListNameLessThan) {
    //     return item->entity()->name().left(5);
    // } else {
    //     return QString();
    // }

    return QString();
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
