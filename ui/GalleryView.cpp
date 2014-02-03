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

void GalleryView::playListChange(PlayList playList)
{
    clear();
    playListAppend(playList);
}

void GalleryView::playListAppend(PlayList appended)
{
    const QSize &galleryItemSize = GlobalConfig::instance()->galleryItemSize();
    int maxColumns = width() / galleryItemSize.width();

    // Add new items
    for (int i = m_scene->items().length(); i < appended.length(); ++i) {
        Image *image = appended.at(i).data();

        // Paint thumbnail
        GalleryItem *item = new GalleryItem(image);

        // Position
        qreal x = i % maxColumns * galleryItemSize.width();
        qreal y = i / maxColumns * galleryItemSize.height();
        item->setPos(x, y);

        connect(this, SIGNAL(currentSelectionChanged(int)),
                item, SLOT(selectionChange(int)));
        m_scene->addItem(item);
    }
}

void GalleryView::mousePressEvent(QMouseEvent *event)
{
    QGraphicsItem *item = itemAt(event->pos());
    if (item) {
        int index = items().length() - items().indexOf(item) - 1;
        emit currentSelectionChanged(index);
    }
}
