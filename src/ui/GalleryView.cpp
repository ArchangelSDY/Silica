#include <QLayout>
#include <QMenu>

#include "../Image.h"
#include "GalleryItem.h"
#include "GalleryView.h"
#include "../GlobalConfig.h"

GalleryView::GalleryView(QWidget *parent) :
    QGraphicsView(parent) ,
    m_navigator(0) ,
    m_playList(0) ,
    m_scene(new QGraphicsScene)
{
    setDragMode(QGraphicsView::RubberBandDrag);
    m_scene->setBackgroundBrush(Qt::gray);
    setScene(m_scene);
}

GalleryView::~GalleryView()
{
    delete m_scene;
}

void GalleryView::setNavigator(Navigator *navigator)
{
    m_navigator = navigator;
}

void GalleryView::clear()
{
    // Clear first
    foreach (const QGraphicsItem *item, m_scene->items()) {
        delete item;
    }
    m_scene->clear();
}

void GalleryView::layout()
{
    if (!isVisible()) {
        return;
    }

    const QList<QGraphicsItem *> items = m_scene->items(Qt::AscendingOrder);
    const QSize &galleryItemSize = GlobalConfig::instance()->galleryItemSize();

    int maxColumns = width() / galleryItemSize.width();
    if (maxColumns == 0) {
        return;
    }

    int maxRows = items.length() / maxColumns + 1;

    for (int i = m_scene->items().length() - 1; i >= 0; --i) {
        QGraphicsItem *item = items[i];

        // Position
        qreal x = i % maxColumns * galleryItemSize.width();
        qreal y = i / maxColumns * galleryItemSize.height();
        item->setPos(x, y);
    }

    QRect newSceneRect(0, 0,
        maxColumns * galleryItemSize.width(),
        maxRows * galleryItemSize.height());
    setSceneRect(newSceneRect);
}

void GalleryView::playListChange(PlayList playList)
{
    clear();
    playListAppend(playList);
}

void GalleryView::playListAppend(PlayList appended)
{
    for (int i = 0; i < appended.length(); ++i) {
        Image *image = appended.at(i).data();

        // Paint thumbnail
        GalleryItem *item = new GalleryItem(image);
        m_scene->addItem(item);
    }

    layout();
}

void GalleryView::mouseDoubleClickEvent(QMouseEvent *)
{
    if (scene()->selectedItems().length() > 0) {
        emit transitToView();
    }
}

void GalleryView::resizeEvent(QResizeEvent *)
{
    layout();
}

void GalleryView::showEvent(QShowEvent *)
{
    layout();
}

void GalleryView::contextMenuEvent(QContextMenuEvent *event)
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

void GalleryView::sortByName()
{
    if (!m_navigator) {
        return;
    }

    PlayList pl = m_navigator->playList();
    pl.sortByName();
    m_navigator->setPlayList(pl);
}

void GalleryView::sortByAspectRatio()
{
    if (!m_navigator) {
        return;
    }

    PlayList pl = m_navigator->playList();
    pl.sortByAspectRatio();
    m_navigator->setPlayList(pl);
}
