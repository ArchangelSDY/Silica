#include <QMenu>

#include "CompactRendererFactory.h"
#include "ImageGalleryItem.h"
#include "ImageGalleryView.h"
#include "LooseRendererFactory.h"

ImageGalleryView::ImageGalleryView(QWidget *parent) :
    GalleryView(parent) ,
    m_navigator(0) ,
    m_playList(0) ,
    m_rendererFactory(new LooseRendererFactory())
{
}

ImageGalleryView::~ImageGalleryView()
{
    delete m_rendererFactory;
}

void ImageGalleryView::setNavigator(Navigator *navigator)
{
    m_navigator = navigator;
}

void ImageGalleryView::playListChange(PlayList playList)
{
    clear();
    playListAppend(playList);
}

void ImageGalleryView::playListAppend(PlayList appended)
{
    for (int i = 0; i < appended.length(); ++i) {
        Image *image = appended.at(i).data();

        // Paint thumbnail
        ImageGalleryItem *item = new ImageGalleryItem(
            image, m_rendererFactory->createRenderer());
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

    QMenu *renderers = menu.addMenu(tr("Layout"));
    renderers->addAction(tr("Loose"), this, SLOT(setLooseRenderer()));
    renderers->addAction(tr("Compact"), this, SLOT(setCompactRenderer()));

    menu.exec(event->globalPos());
}

void ImageGalleryView::sortByName()
{
    if (!m_navigator) {
        return;
    }

    PlayList pl(*m_navigator->playList());
    pl.sortByName();
    m_navigator->setPlayList(pl);
}

void ImageGalleryView::sortByAspectRatio()
{
    if (!m_navigator) {
        return;
    }

    PlayList pl(*m_navigator->playList());
    pl.sortByAspectRatio();
    m_navigator->setPlayList(pl);
}

void ImageGalleryView::setRendererFactory(AbstractRendererFactory *factory)
{
    delete m_rendererFactory;
    m_rendererFactory = factory;

    // Update renderers for each item
    foreach (QGraphicsItem *item, scene()->items()) {
        ImageGalleryItem *imageItem = static_cast<ImageGalleryItem *>(item);
        imageItem->setRenderer(m_rendererFactory->createRenderer());
    }
    update();
}

void ImageGalleryView::setLooseRenderer()
{
    setRendererFactory(new LooseRendererFactory());
}

void ImageGalleryView::setCompactRenderer()
{
    setRendererFactory(new CompactRendererFactory());
}
