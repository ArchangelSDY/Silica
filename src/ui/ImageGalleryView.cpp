#ifdef ENABLE_OPENGL
#include <QGL>
#include <QOpenGLWidget>
#endif

#include <QContextMenuEvent>
#include <QMenu>
#include <QMessageBox>
#include <QRunnable>
#include <QThreadPool>
#include <QtConcurrent>

#include "image/metadata/ImageMetadataConstants.h"
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

ImageGalleryView::ImageGalleryView(QWidget *parent) :
    GalleryView(parent) ,
    m_playList(0) ,
    m_rankFilterMenuManager(0)
{
#ifdef ENABLE_OPENGL
    m_view->setViewport(new QOpenGLWidget(this));
#endif

    setRendererFactory(new LooseRendererFactory());
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

void ImageGalleryView::setPlayListEntity(PlayListEntity *playListEntity)
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
    renderers->addAction(tr("Loose"), this, SLOT(setLooseRenderer()));
    renderers->addAction(tr("Compact"), this, SLOT(setCompactRenderer()));
    renderers->addAction(tr("Waterfall"), this, SLOT(setWaterfallRenderer()));

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
        menu->addAction(tr("Remove"), this, SLOT(removeSelected()));
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
            entity->provider()->updateEntity(entity);
        });
    }
}

void ImageGalleryView::removeSelected()
{
    QList<GalleryItem *> selectedItems = selectedGalleryItems();

    QString msg = tr("Remove %1 images?").arg(selectedItems.count());
    if (QMessageBox::question(
            this, tr("Images Remove"), msg) == QMessageBox::Yes) {
        ImageList toRemoves;
        foreach (GalleryItem *item, selectedItems) {
            ImageGalleryItem *galleryItem =
                static_cast<ImageGalleryItem *>(item);
            ImagePtr toRemove = galleryItem->image();

            toRemoves << toRemove;
            m_playList->removeOne(toRemove);
        }

        // TODO
        // // Sync to record if any
        // PlayListRecord *record = m_playList->record();
        // if (record && !toRemoves.isEmpty()) {
        //     QtConcurrent::run([record, toRemoves]() {
        //         record->removeImages(toRemoves);
        //     });
        // }
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

class BackgroundGroupTask : public QObject, public QRunnable
{
    Q_OBJECT
public:
    BackgroundGroupTask(QSharedPointer<PlayList> pl, AbstractPlayListGrouper *grouper) :
        m_pl(pl),
        m_grouper(grouper) {}

    void run()
    {
        m_pl->groupBy(m_grouper);
    }

private:
    QSharedPointer<PlayList> m_pl;
    AbstractPlayListGrouper *m_grouper;
};

void ImageGalleryView::groupBy(AbstractPlayListGrouper *grouper)
{
    enableGrouping();

    if (m_playList) {
        BackgroundGroupTask *task = new BackgroundGroupTask(m_playList, grouper);
        task->setAutoDelete(true);
        connect(task, SIGNAL(destroyed()), &m_groupingProgress, SLOT(stop()));
        QThreadPool::globalInstance()->start(task);

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
    }
}


#include "ImageGalleryView.moc"
