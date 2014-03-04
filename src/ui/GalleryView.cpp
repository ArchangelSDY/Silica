#include <QGraphicsItem>

#include "CompactRendererFactory.h"
#include "GalleryItem.h"
#include "GalleryView.h"
#include "GlobalConfig.h"
#include "LooseRendererFactory.h"

GalleryView::GalleryView(QWidget *parent) :
    QGraphicsView(parent) ,
    m_scene(new QGraphicsScene)
{
    setDragMode(QGraphicsView::RubberBandDrag);

    QPalette palette;
    palette.setBrush(QPalette::Background, QColor("#323A44"));
    palette.setBrush(QPalette::Foreground, QColor("#71929E"));
    m_scene->setPalette(palette);
    m_scene->setBackgroundBrush(palette.background());

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

void GalleryView::resizeEvent(QResizeEvent *)
{
    layout();
}

void GalleryView::showEvent(QShowEvent *)
{
    layout();
}

void GalleryView::mouseDoubleClickEvent(QMouseEvent *)
{
    if (scene()->selectedItems().length() > 0) {
        emit enterItem();
    }
}

void GalleryView::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return &&
            scene()->selectedItems().length() > 0) {
        event->accept();
        emit enterItem();
    } else {
        QGraphicsView::keyPressEvent(event);
    }
}

void GalleryView::setRendererFactory(AbstractRendererFactory *factory)
{
    delete m_rendererFactory;
    m_rendererFactory = factory;

    // Update renderers for each item
    foreach (QGraphicsItem *item, scene()->items()) {
        GalleryItem *galleryItem = static_cast<GalleryItem *>(item);
        galleryItem->setRendererFactory(m_rendererFactory);
    }

    update();
}

void GalleryView::setLooseRenderer()
{
    setRendererFactory(new LooseRendererFactory());
}

void GalleryView::setCompactRenderer()
{
    setRendererFactory(new CompactRendererFactory());
}
