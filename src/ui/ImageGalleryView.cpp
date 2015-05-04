#ifdef ENABLE_OPENGL
#include <QGL>
#include <QOpenGLWidget>
#endif

#include <QMenu>
#include <QMessageBox>
#include <QRunnable>
#include <QThreadPool>

#include "CompactRendererFactory.h"
#include "ImageGalleryItem.h"
#include "ImageGalleryView.h"
#include "LooseRendererFactory.h"
#include "Navigator.h"
#include "PlayListRecord.h"

ImageGalleryView::ImageGalleryView(QWidget *parent) :
    GalleryView(parent) ,
    m_playList(0) ,
    m_rankFilterMenuManager(0) ,
    m_groupMode(GroupByThumbHist)
{
#ifdef ENABLE_OPENGL
    m_view->setViewport(new QOpenGLWidget(this));
#endif

    setRendererFactory(new LooseRendererFactory());
}

ImageGalleryView::~ImageGalleryView()
{
    if (m_rankFilterMenuManager) {
        delete m_rankFilterMenuManager;
    }
}

void ImageGalleryView::setPlayList(PlayList *playList)
{
    m_playList = playList;

    if (m_rankFilterMenuManager) {
        delete m_rankFilterMenuManager;
    }
    m_rankFilterMenuManager = new RankFilterMenuManager(&m_playList, this);
}

void ImageGalleryView::playListChange(PlayList *playList)
{
    if (playList && playList != m_playList) {
        setPlayList(playList);
    }

    clear();
    playListAppend(0);
}

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
    sorts->addAction(tr("Aspect Ratio"), this, SLOT(sortByAspectRatio()));

    QMenu *groups = menu->addMenu(tr("Group By"));
    groups->addAction(tr("None"), this, SLOT(disableGrouping()));
    groups->addAction(tr("Thumb Hist"), this, SLOT(groupByThumbHist()));

    menu->addMenu(m_rankFilterMenuManager->menu());

    QMenu *renderers = menu->addMenu(tr("Layout"));
    renderers->addAction(tr("Loose"), this, SLOT(setLooseRenderer()));
    renderers->addAction(tr("Compact"), this, SLOT(setCompactRenderer()));
    renderers->addAction(tr("Waterfall"), this, SLOT(setWaterfallRenderer()));

    QList<GalleryItem *> selectedItems = selectedGalleryItems();
    if (m_playList && m_playList->record()) {
        QAction *actSetAsCover =
            menu->addAction(tr("Set As Cover"), this, SLOT(setAsCover()));
        if (selectedItems.count() == 0) {
            actSetAsCover->setEnabled(false);
        }
    }

    if (!selectedItems.isEmpty()) {
        menu->addAction(tr("Remove"), this, SLOT(removeSelected()));
    }

    return menu;
}

void ImageGalleryView::sortByName()
{
    if (!m_playList) {
        return;
    }

    m_playList->sortByName();
}

void ImageGalleryView::sortByAspectRatio()
{
    if (!m_playList) {
        return;
    }

    m_playList->sortByAspectRatio();
}

void ImageGalleryView::setAsCover()
{
    QList<GalleryItem *> selectedItems = selectedGalleryItems();
    if (m_playList && selectedItems.count() > 0) {
        ImageGalleryItem *item =
            static_cast<ImageGalleryItem *>(selectedItems[0]);
        PlayListRecord *record = m_playList->record();

        if (record) {
             record->setCoverPath(item->image()->thumbnailPath());
             record->save();
        }
    }
}

void ImageGalleryView::addToBasket()
{
    QList<GalleryItem *> selectedItems = selectedGalleryItems();
    for (int i = 0; i < selectedItems.count(); ++i) {
        ImageGalleryItem *item =
            static_cast<ImageGalleryItem *>(selectedItems[i]);

        ImagePtr image = item->image();
        *(Navigator::instance()->basket()) << image;
    }
}

void ImageGalleryView::removeSelected()
{
    QList<GalleryItem *> selectedItems = selectedGalleryItems();

    QString msg = tr("Remove %1 images?").arg(selectedItems.count());
    if (QMessageBox::question(
            this, tr("Images Remove"), msg) == QMessageBox::Yes) {
        foreach (GalleryItem *item, selectedItems) {
            ImageGalleryItem *galleryItem =
                static_cast<ImageGalleryItem *>(item);
            ImagePtr toRemove = galleryItem->image();

            m_playList->removeOne(toRemove);

            // Sync to record if any
            PlayListRecord *record = m_playList->record();
            if (record) {
                record->removeImage(toRemove);
            }
        }
    }
}

QString ImageGalleryView::groupForItem(GalleryItem *rawItem)
{
    ImageGalleryItem *item = static_cast<ImageGalleryItem *>(rawItem);
    const ImagePtr &image = item->image();
    if (!image.isNull()) {
        int group = m_playList->groupForImage(image.data());
        return QString(tr("Group %1").arg(group));
    } else {
        return QString();
    }
}

class GroupByThumbHistTask : public QObject, public QRunnable
{
    Q_OBJECT
public:
    GroupByThumbHistTask(PlayList *pl) : m_pl(pl) {}
    void run()
    {
        m_pl->groupByThumbHist();;
        m_pl->sortByGroup();
    }

private:
    PlayList *m_pl;
};

void ImageGalleryView::groupByThumbHist()
{
    m_groupMode = GroupByThumbHist;
    enableGrouping();

    if (m_playList) {
        GroupByThumbHistTask *task = new GroupByThumbHistTask(m_playList);
        task->setAutoDelete(true);
        connect(task, SIGNAL(destroyed()), &m_groupingProgress, SLOT(stop()));
        QThreadPool::globalInstance()->start(task);
        m_groupingProgress.reset();
        m_groupingProgress.start();
    }
}


#include "ImageGalleryView.moc"
