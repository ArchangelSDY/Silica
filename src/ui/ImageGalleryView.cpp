#include <QMenu>

#include "CompactRendererFactory.h"
#include "ImageGalleryItem.h"
#include "ImageGalleryView.h"
#include "LooseRendererFactory.h"

ImageGalleryView::ImageGalleryView(QWidget *parent) :
    GalleryView(parent) ,
    m_navigator(0) ,
    m_playList(0) ,
    m_rankFilterMenuManager(new RankFilterMenuManager(&m_navigator, this))
{
    m_rendererFactory = new LooseRendererFactory();
}

ImageGalleryView::~ImageGalleryView()
{
    delete m_rendererFactory;
}

void ImageGalleryView::setNavigator(Navigator *navigator)
{
    m_navigator = navigator;
}

void ImageGalleryView::playListChange()
{
    clear();

    playListAppend(0);
}

void ImageGalleryView::playListAppend(int start)
{
    PlayList *pl = m_navigator->playList();
    for (int i = start; i < pl->count(); ++i) {
        Image *image = pl->at(i).data();

        // Paint thumbnail
        ImageGalleryItem *item = new ImageGalleryItem(image, m_rendererFactory);
        m_scene->addItem(item);
    }

    layout();
}

void ImageGalleryView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);

    QMenu *sorts = menu.addMenu(tr("Sort By"));
    sorts->addAction(tr("Name"), this, SLOT(sortByName()));
    sorts->addAction(tr("Aspect Ratio"), this, SLOT(sortByAspectRatio()));

    menu.addMenu(m_rankFilterMenuManager->menu());

    QMenu *renderers = menu.addMenu(tr("Layout"));
    renderers->addAction(tr("Loose"), this, SLOT(setLooseRenderer()));
    renderers->addAction(tr("Compact"), this, SLOT(setCompactRenderer()));

    if (m_navigator->playList()->record() != 0) {
        QAction *actSetAsCover =
            menu.addAction(tr("Set As Cover"), this, SLOT(setAsCover()));
        if (scene()->selectedItems().count() == 0) {
            actSetAsCover->setEnabled(false);
        }
    }

    menu.exec(event->globalPos());
}

void ImageGalleryView::mousePressEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::RightButton) {
        ev->accept();
        return;
    }

    QGraphicsView::mousePressEvent(ev);
}

void ImageGalleryView::sortByName()
{
    if (!m_navigator || !m_navigator->playList()) {
        return;
    }

    m_navigator->playList()->sortByName();
}

void ImageGalleryView::sortByAspectRatio()
{
    if (!m_navigator || !m_navigator->playList()) {
        return;
    }

    m_navigator->playList()->sortByAspectRatio();
}

void ImageGalleryView::setAsCover()
{
    if (scene()->selectedItems().count() > 0) {
        ImageGalleryItem *item =
            static_cast<ImageGalleryItem *>(scene()->selectedItems()[0]);
        PlayListRecord *record = m_navigator->playList()->record();

        if (record) {
             record->setCoverPath(item->image()->thumbnailPath());
             record->save();
        }
    }
}
