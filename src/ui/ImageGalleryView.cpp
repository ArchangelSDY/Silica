#ifdef ENABLE_OPENGL
#include <QOpenGLWidget>
#endif

#include <QContextMenuEvent>
#include <QMenu>
#include <QMessageBox>
#include <QRunnable>
#include <QThreadPool>
#include <QtConcurrent>

#include "image/metadata/ImageMetadataConstants.h"
#include "image/Image.h"
#include "image/ImageSource.h"
#include "playlist/group/PlayListImageMetadataGrouper.h"
#include "playlist/group/PlayListImageThumbnailHistogramGrouper.h"
#include "playlist/sort/PlayListImageAspectRatioSorter.h"
#include "playlist/sort/PlayListImageNameSorter.h"
#include "playlist/sort/PlayListImageSizeSorter.h"
#include "playlist/sort/PlayListImageUrlSorter.h"
#include "playlist/PlayListEntity.h"
#include "playlist/PlayListProvider.h"
#include "ui/ImageGalleryItem.h"
#include "ui/ImageGalleryView.h"
#include "ui/renderers/CompactRendererFactory.h"
#include "ui/renderers/LooseRendererFactory.h"
#include "ui/renderers/WaterfallRendererFactory.h"
#include "GlobalConfig.h"

ImageGalleryView::ImageGalleryView(QWidget *parent) :
    GalleryView(parent) ,
    m_playList(nullptr) ,
    m_playListEntity(nullptr) ,
    m_rankFilterMenuManager(nullptr)
{
#ifdef ENABLE_OPENGL
    m_view->setViewport(new QOpenGLWidget(this));
#endif

    setRendererFactory(new LooseRendererFactory());

    connect(&m_groupByWatcher, &QFutureWatcher<void>::finished, &m_groupingProgress, &TaskProgress::stop);
}

ImageGalleryView::~ImageGalleryView()
{
}

void ImageGalleryView::setPlayList(QSharedPointer<PlayList> playList)
{
    m_playList = playList;
    m_rankFilterMenuManager.reset(new RankFilterMenuManager(m_playList));
    reload();
}

void ImageGalleryView::setPlayListEntity(QSharedPointer<PlayListEntity> playListEntity)
{
    m_playListEntity = playListEntity;
}

void ImageGalleryView::reload()
{
    clear();
    playListAppend(0);
}

void ImageGalleryView::playListItemChange(int index)
{
    replaceItem(index, new ImageGalleryItem(m_playList->at(index), rendererFactory()));
    scheduleLayout();
}

// void ImageGalleryView::playListChange(QSharedPointer<PlayList> playList)
// {
//     if (playList && playList != m_playList) {
//         setPlayList(playList);
//     }
// 
//     clear();
//     playListAppend(0);
// }

void ImageGalleryView::playListAppend(int start)
{
    incrItemsToLoad(m_playList->count() - start);

    for (int i = start; i < m_playList->count(); ++i) {
        ImagePtr image = m_playList->at(i);

        // Paint thumbnail
        ImageGalleryItem *item = new ImageGalleryItem(image, rendererFactory());
        addItem(item);
    }

    scheduleLayout();
}

void ImageGalleryView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = createContextMenu();
    menu->exec(event->globalPos());
}

QMenu *ImageGalleryView::createContextMenu()
{
    QMenu *menu = new QMenu(this);

    QMenu *sorts = menu->addMenu(tr("Sort By"));
    sorts->addAction(tr("Name"), this, SLOT(sortByName()));
    sorts->addAction(tr("Url"), this, SLOT(sortByUrl()));
    sorts->addAction(tr("Aspect Ratio"), this, SLOT(sortByAspectRatio()));
    sorts->addAction(tr("Size"), this, SLOT(sortBySize()));

    QMenu *groups = menu->addMenu(tr("Group By"));
    groups->addAction(tr("None"), this, SLOT(disableGrouping()));
    groups->addAction(tr("Title"), this, [this]() {
        this->groupBy(new PlayListImageMetadataGrouper(ImageMetadataConstants::DC_TITLE));
    });
    groups->addAction(tr("Label"), this, [this]() {
        this->groupBy(new PlayListImageMetadataGrouper(ImageMetadataConstants::XMP_LABEL));
    });
    groups->addAction(tr("Thumb Hist"), this, [this]() {
        this->groupBy(new PlayListImageThumbnailHistogramGrouper());
    });

    menu->addMenu(m_rankFilterMenuManager->menu());

    QMenu *renderers = menu->addMenu(tr("Layout"));
    renderers->addAction(tr("Loose"), [this]() {
        this->setRendererFactory(new LooseRendererFactory());
    });
    renderers->addAction(tr("Compact"), [this]() {
        this->setRendererFactory(new CompactRendererFactory());
    });
    renderers->addAction(tr("Waterfall (6)"), [this]() {
        this->setRendererFactory(new WaterfallRendererFactory(6));
    });
    renderers->addAction(tr("Waterfall (4)"), [this]() {
        this->setRendererFactory(new WaterfallRendererFactory(4));
    });
    renderers->addAction(tr("Waterfall (3)"), [this]() {
        this->setRendererFactory(new WaterfallRendererFactory(3));
    });
    renderers->addAction(tr("Waterfall (2)"), [this]() {
        this->setRendererFactory(new WaterfallRendererFactory(2));
    });

    QList<GalleryItem *> selectedItems = selectedGalleryItems();

    if (m_playListEntity && m_playListEntity->provider()->supportsOption(PlayListProviderOption::UpdateEntity)) {
        QAction *actSetAsCover =
            menu->addAction(tr("Set As Cover"), this, SLOT(setAsCover()));
        if (selectedItems.count() == 0) {
            actSetAsCover->setEnabled(false);
        }
    }

    if (!selectedItems.isEmpty()) {
        menu->addAction(tr("Refresh"), this, SLOT(refreshSelected()));
        auto removeAct = menu->addAction(tr("Remove"), this, SLOT(removeSelected()));
        if (m_playListEntity && !m_playListEntity->supportsOption(PlayListEntityOption::RemoveImageUrls)) {
            removeAct->setEnabled(false);
        }
    }

    return menu;
}

void ImageGalleryView::sortByName()
{
    if (!m_playList) {
        return;
    }

    PlayListImageNameSorter sorter;
    m_playList->sortBy(&sorter);
}

void ImageGalleryView::sortByUrl()
{
    if (!m_playList) {
        return;
    }

    PlayListImageUrlSorter sorter;
    m_playList->sortBy(&sorter);
}

void ImageGalleryView::sortByAspectRatio()
{
    if (!m_playList) {
        return;
    }

    PlayListImageAspectRatioSorter sorter;
    m_playList->sortBy(&sorter);
}

void ImageGalleryView::sortBySize()
{
    if (!m_playList) {
        return;
    }

    PlayListImageSizeSorter sorter;
    m_playList->sortBy(&sorter);
}

void ImageGalleryView::setAsCover()
{
    QList<GalleryItem *> selectedItems = selectedGalleryItems();
    if (m_playListEntity && selectedItems.count() > 0) {
        ImageGalleryItem *item = static_cast<ImageGalleryItem *>(selectedItems[0]);
        m_playListEntity->setCoverImagePath(item->image()->thumbnailPath());

        auto entity = m_playListEntity;
        QtConcurrent::run([entity]() {
            entity->provider()->updateEntity(entity.data());
        });
    }
}

void ImageGalleryView::removeSelected()
{
    QList<GalleryItem *> selectedItems = selectedGalleryItems();

    QString msg = tr("Remove %1 images from playlist?").arg(selectedItems.count());
    if (QMessageBox::question(
            this, tr("Remove images"), msg) == QMessageBox::Yes) {
        QList<QUrl> toRemoveUrls;
        foreach (GalleryItem *item, selectedItems) {
            ImageGalleryItem *galleryItem =
                static_cast<ImageGalleryItem *>(item);
            ImagePtr toRemove = galleryItem->image();

            toRemoveUrls << toRemove->source()->url();
            m_playList->removeOne(toRemove);
        }

        // Sync to entity if any
        if (m_playListEntity && !toRemoveUrls.isEmpty()) {
            Q_ASSERT(m_playListEntity->supportsOption(PlayListEntityOption::RemoveImageUrls));
            auto playListEntity = m_playListEntity;
            QtConcurrent::run([playListEntity, toRemoveUrls]() {
                playListEntity->removeImageUrls(toRemoveUrls);
            });
        }
    }
}

void ImageGalleryView::refreshSelected()
{
    QList<GalleryItem *> selectedItems = selectedGalleryItems();

    for (GalleryItem *rawItem : selectedItems) {
        ImageGalleryItem *item = static_cast<ImageGalleryItem *>(rawItem);
        ImagePtr image = item->image();
        image->makeThumbnail();
    }
}

QString ImageGalleryView::groupForItem(GalleryItem *rawItem)
{
    ImageGalleryItem *item = static_cast<ImageGalleryItem *>(rawItem);
    ImagePtr image = item->image();
    if (!image.isNull()) {
        return m_playList->groupNameOf(image);
    } else {
        return QStringLiteral("");
    }
}

void ImageGalleryView::groupBy(AbstractPlayListGrouper *grouper)
{
    enableGrouping();

    if (m_playList) {
        if (m_playListEntity) {
            QString key = QStringLiteral("ImageGalleryView::groupBy_%1_%2")
                .arg(m_playListEntity->name()) // TODO: It seems we need a unique id here
                .arg(QUuid::createUuid().toString());
            m_groupingProgress.setKey(key);
            m_groupingProgress.setEstimateEnabled(true);
            m_groupingProgress.setMaximum(36);
        } else {
            m_groupingProgress.setKey(QString());
            m_groupingProgress.setEstimateEnabled(false);
            m_groupingProgress.setMaximum(0);
        }

        m_groupingProgress.reset();
        m_groupingProgress.start();

        auto playList = m_playList;
        auto future = QtConcurrent::run([playList, grouper]() {
            playList->groupBy(grouper);
        });
        m_groupByWatcher.setFuture(future);
    }
}
