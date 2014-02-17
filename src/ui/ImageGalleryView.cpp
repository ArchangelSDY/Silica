#include <QMenu>

#include "GalleryItem.h"
#include "ImageGalleryItemModel.h"
#include "ImageGalleryView.h"

ImageGalleryView::ImageGalleryView(QWidget *parent) :
    GalleryView(parent) ,
    m_navigator(0) ,
    m_playList(0)
{
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
        ImageGalleryItemModel *model = new ImageGalleryItemModel(image);
        GalleryItem *item = new GalleryItem(model);
        m_scene->addItem(item);
    }

    layout();
}

void ImageGalleryView::mouseDoubleClickEvent(QMouseEvent *)
{
    if (scene()->selectedItems().length() > 0) {
        emit transitToView();
    }
}

void ImageGalleryView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    QMenu *sorts = menu.addMenu("Sort By");
    QAction *byNameAct = sorts->addAction("Name");
    connect(byNameAct, SIGNAL(triggered()),
            this, SLOT(sortByName()));
    QAction *byAspectRatioAct = sorts->addAction("Aspect Ratio");
    connect(byAspectRatioAct, SIGNAL(triggered()),
            this, SLOT(sortByAspectRatio()));

    menu.exec(event->globalPos());
}

void ImageGalleryView::sortByName()
{
    if (!m_navigator) {
        return;
    }

    PlayList pl = m_navigator->playList();
    pl.sortByName();
    m_navigator->setPlayList(pl);
}

void ImageGalleryView::sortByAspectRatio()
{
    if (!m_navigator) {
        return;
    }

    PlayList pl = m_navigator->playList();
    pl.sortByAspectRatio();
    m_navigator->setPlayList(pl);
}
