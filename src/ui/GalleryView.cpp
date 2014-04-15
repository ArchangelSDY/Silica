#include <QGLWidget>
#include <QGraphicsItem>

#include "CompactRendererFactory.h"
#include "GalleryItem.h"
#include "GalleryView.h"
#include "GlobalConfig.h"
#include "LooseRendererFactory.h"

static const qreal GROUP_PADDING_ROW = 0.1;

GalleryView::GalleryView(QWidget *parent) :
    QGraphicsView(parent) ,
    m_scene(new QGraphicsScene) ,
    m_enableGrouping(false)
{
    setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
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
    if (!isVisible() || m_scene->items().length() == 0) {
        return;
    }

    const QSize &galleryItemSize = GlobalConfig::instance()->galleryItemSize();
    int maxColumns = width() / galleryItemSize.width();
    if (maxColumns == 0) {
        return;
    }

    QList<QGraphicsItem *> items = m_scene->items(Qt::AscendingOrder);
    if (m_enableGrouping) {
        sortByGroup(&items);
    }

    qreal curRow = 0, curColumn = -1;
    QString curGroup = groupForItem(static_cast<GalleryItem *>(items[0]));
    for (int i = 0; i < items.length(); ++i) {
        GalleryItem *item = static_cast<GalleryItem *>(items[i]);

        bool shouldBreakLine = false;
        // Break line if exceeds columns limit
        if (curColumn == maxColumns - 1) {
            shouldBreakLine = true;
        }

        // Break line if group changs
        if (m_enableGrouping && groupForItem(item) != curGroup) {
            curRow += GROUP_PADDING_ROW;    // Add additional padding row

            shouldBreakLine = true;
        }
        curGroup = groupForItem(item);

        if (shouldBreakLine) {
            curRow += 1;
            curColumn = 0;
        } else {
            curColumn += 1;
        }

        qreal x = curColumn * galleryItemSize.width();
        qreal y = curRow * galleryItemSize.height();
        item->setPos(x, y);
    }

    QRectF newSceneRect(0, 0,
        maxColumns * galleryItemSize.width(),
        (curRow + 1) * galleryItemSize.height());
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

    viewport()->update();
}

void GalleryView::setLooseRenderer()
{
    setRendererFactory(new LooseRendererFactory());
}

void GalleryView::setCompactRenderer()
{
    setRendererFactory(new CompactRendererFactory());
}

void GalleryView::enableGrouping()
{
    m_enableGrouping = true;
    layout();
}

void GalleryView::disableGrouping()
{
    m_enableGrouping = false;
    layout();
}
