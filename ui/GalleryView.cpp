#include <QLayout>

#include "../Image.h"
#include "GalleryItem.h"
#include "GalleryView.h"
#include "../GlobalConfig.h"

GalleryView::GalleryView(QWidget *parent) :
    QGraphicsView(parent) ,
    m_playList(0) ,
    m_scene(new QGraphicsScene)
{
    m_scene->setBackgroundBrush(Qt::gray);
    setScene(m_scene);
}

GalleryView::~GalleryView()
{
    delete m_scene;
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
    // Add new items
    for (int i = m_scene->items().length(); i < appended.length(); ++i) {
        Image *image = appended.at(i).data();

        // Paint thumbnail
        GalleryItem *item = new GalleryItem(image);
        m_scene->addItem(item);
    }

    layout();
}

void GalleryView::mousePressEvent(QMouseEvent *event)
{
    scene()->clearSelection();
    QGraphicsItem *item = itemAt(event->pos());
    if (item) {
        item->setSelected(true);
    }
}

void GalleryView::resizeEvent(QResizeEvent *)
{
    layout();
}

void GalleryView::showEvent(QShowEvent *)
{
    this->parentWidget()->layout()->update();
    layout();
}
