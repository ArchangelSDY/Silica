#include <QMenu>
#include <QMessageBox>

#include "CompactRendererFactory.h"
#include "ImageGalleryItem.h"
#include "ImageGalleryView.h"
#include "LooseRendererFactory.h"
#include "Navigator.h"
#include "PlayListRecord.h"

ImageGalleryView::ImageGalleryView(QWidget *parent) :
    GalleryView(parent) ,
    m_playList(0) ,
    m_rankFilterMenuManager(0)
{
    m_rendererFactory = new LooseRendererFactory();
}

ImageGalleryView::~ImageGalleryView()
{
    delete m_rendererFactory;
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
    for (int i = start; i < m_playList->count(); ++i) {
        ImagePtr image = m_playList->at(i);

        // Paint thumbnail
        ImageGalleryItem *item = new ImageGalleryItem(image, m_rendererFactory);
        m_scene->addItem(item);
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

    menu->addMenu(m_rankFilterMenuManager->menu());

    QMenu *renderers = menu->addMenu(tr("Layout"));
    renderers->addAction(tr("Loose"), this, SLOT(setLooseRenderer()));
    renderers->addAction(tr("Compact"), this, SLOT(setCompactRenderer()));
    renderers->addAction(tr("Waterfall"), this, SLOT(setWaterfallRenderer()));

    if (m_playList && m_playList->record()) {
        QAction *actSetAsCover =
            menu->addAction(tr("Set As Cover"), this, SLOT(setAsCover()));
        if (scene()->selectedItems().count() == 0) {
            actSetAsCover->setEnabled(false);
        }
    }

    if (!scene()->selectedItems().isEmpty()) {
        menu->addAction(tr("Remove"), this, SLOT(removeSelected()));
    }

    return menu;
}

void ImageGalleryView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        event->accept();
        return;
    }

    QGraphicsView::mousePressEvent(event);
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
    if (m_playList && scene()->selectedItems().count() > 0) {
        ImageGalleryItem *item =
            static_cast<ImageGalleryItem *>(scene()->selectedItems()[0]);
        PlayListRecord *record = m_playList->record();

        if (record) {
             record->setCoverPath(item->image()->thumbnailPath());
             record->save();
        }
    }
}

void ImageGalleryView::addToBasket()
{
    QList<QGraphicsItem *> selectedItems = scene()->selectedItems();
    for (int i = 0; i < selectedItems.count(); ++i) {
        ImageGalleryItem *item =
            static_cast<ImageGalleryItem *>(selectedItems[i]);

        ImagePtr image = item->image();
        *(Navigator::instance()->basket()) << image;
    }
}

void ImageGalleryView::removeSelected()
{
    QList<QGraphicsItem *> selectedItems = scene()->selectedItems();

    QString msg = tr("Remove %1 images?").arg(selectedItems.count());
    if (QMessageBox::question(
            this, tr("Images Remove"), msg) == QMessageBox::Yes) {
        foreach (QGraphicsItem *item, selectedItems) {
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
