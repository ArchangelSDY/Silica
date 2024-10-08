#include "PlayListGalleryView.h"

#include <QClipboard>
#include <QContextMenuEvent>
#include <QGuiApplication>
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
    m_groupLessThan(playListNameLessThan)
{
    setRendererFactory(new CompactRendererFactory());
}

void PlayListGalleryView::setPlayListEntities(QList<QSharedPointer<PlayListEntity> > entities)
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
    connect(plrCreatorMap, &QSignalMapper::mappedInt,
        this, &PlayListGalleryView::createPlayListEntity);

    if (selectedItems.count() > 0) {
        if (selectedItems.count() == 1) {
            QAction *actCopyName = menu.addAction(tr("Copy Name"), this, SLOT(copyNameSelectedItem()));
        }

        QAction *actRename = menu.addAction(tr("Rename"), this, SLOT(renameSelectedItem()));
        QAction *actRemove = menu.addAction(tr("Remove"), this, SLOT(removeSelectedItems()));

        for (GalleryItem *item : selectedItems) {
            PlayListGalleryItem *playListItem = static_cast<PlayListGalleryItem *>(item);
            auto entity = playListItem->entity();
            if (!entity->provider()->supportsOption(PlayListProviderOption::UpdateEntity)) {
                actRename->setEnabled(false);
            }
            if (!entity->provider()->supportsOption(PlayListProviderOption::RemoveEntity)) {
                actRemove->setEnabled(false);
            }
        }
    }

    menu.addSeparator();

    QMenu *renderers = menu.addMenu(tr("Layout"));
    renderers->addAction(tr("Loose"), [this]() {
        this->setRendererFactory(new LooseRendererFactory());
    });
    renderers->addAction(tr("Compact"), [this]() {
        this->setRendererFactory(new CompactRendererFactory());
    });

    QMenu *groups = menu.addMenu(tr("Group By"));
    QAction *actDisableGrouping =
        groups->addAction(tr("None"), this, SLOT(disableGrouping()));
    actDisableGrouping->setCheckable(true);
    actDisableGrouping->setChecked(!m_enableGrouping);
    QAction *actGroupByName =
        groups->addAction(tr("Name Prefix"), this, SLOT(groupByNamePrefix()));
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

void PlayListGalleryView::copyNameSelectedItem()
{
    QList<GalleryItem *> selectedItems = selectedGalleryItems();
    if (selectedItems.count() > 0) {
        PlayListGalleryItem *item =
            static_cast<PlayListGalleryItem *>(selectedItems[0]);
        auto entity = item->entity();
        auto name = entity->name();

        auto clipboard = QGuiApplication::clipboard();
        clipboard->setText(name);
    }
}

void PlayListGalleryView::renameSelectedItem()
{
    QList<GalleryItem *> selectedItems = selectedGalleryItems();
    if (selectedItems.count() > 0) {
        PlayListGalleryItem *item =
            static_cast<PlayListGalleryItem *>(selectedItems[0]);
        auto entity = item->entity();

        QString newName = QInputDialog::getText(
            0, "Rename PlayList", "New Name",
            QLineEdit::Normal, entity->name());

         if (!newName.isEmpty()) {
             entity->setName(newName);
             entity->provider()->updateEntity(entity.data());
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
                    entity->provider()->removeEntity(entity.data());
                });
            }
        }
    }
}

QString PlayListGalleryView::groupForItem(GalleryItem *rawItem)
{
     PlayListGalleryItem *item = static_cast<PlayListGalleryItem *>(rawItem);
     if (m_groupLessThan == playListNameLessThan) {
         return item->entity()->name().left(5);
     } else {
         return QString();
     }
}

void PlayListGalleryView::sortItemByGroup(QList<GalleryItem *> *items)
{
    std::sort(items->begin(), items->end(), m_groupLessThan);
}

void PlayListGalleryView::groupByNamePrefix()
{
    m_groupLessThan = playListNameLessThan;
    enableGrouping();
}
