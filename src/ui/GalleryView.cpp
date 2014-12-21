#ifdef ENABLE_OPENGL
#include <QGL>
#include <QOpenGLWidget>
#endif

#include <QGraphicsItem>

#include "AbstractGalleryViewRenderer.h"
#include "CompactRendererFactory.h"
#include "GalleryItem.h"
#include "GalleryView.h"
#include "GlobalConfig.h"

#include "CompactRendererFactory.h"
#include "LooseRendererFactory.h"
#include "WaterfallRendererFactory.h"

GalleryView::GalleryView(QWidget *parent) :
    QGraphicsView(parent) ,
    m_scene(new QGraphicsScene) ,
    m_enableGrouping(false) ,
    m_layoutNeeded(true) ,
    m_loadingItemsCount(0)
{
#ifdef ENABLE_OPENGL
    // setViewport(new QOpenGLWidget(QGLFormat(QGL::SampleBuffers)));
    setViewport(new QOpenGLWidget(this));
#endif

    setDragMode(QGraphicsView::RubberBandDrag);

    QPalette palette;
    palette.setBrush(QPalette::Background, QColor("#323A44"));
    palette.setBrush(QPalette::Foreground, QColor("#71929E"));
    m_scene->setPalette(palette);
    m_scene->setBackgroundBrush(palette.background());

    setScene(m_scene);

    m_layoutTimer.setSingleShot(false);
    connect(&m_layoutTimer, SIGNAL(timeout()), this, SLOT(layout()));
    m_layoutTimer.start(GalleryView::LAYOUT_INTERVAL);
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
    m_loadingItemsCount = 0;
}

void GalleryView::layout()
{
    if (!m_layoutNeeded) {
        return;
    }

    if (!isVisible() || m_scene->items().length() == 0) {
        return;
    }

    QList<QGraphicsItem *> items = m_scene->items(Qt::AscendingOrder);

    QStringList itemGroups;
    if (m_enableGrouping) {
        sortItemByGroup(&items);

        foreach (QGraphicsItem *item, items) {
            itemGroups << groupForItem(item);
        }
    }

    AbstractGalleryViewRenderer *renderer =
        m_rendererFactory->createViewRenderer(m_scene);
    renderer->layout(items, itemGroups, geometry());
    delete renderer;

    m_layoutNeeded = false;
}

void GalleryView::scheduleLayout()
{
    m_layoutNeeded = true;
}

void GalleryView::resizeEvent(QResizeEvent *)
{
    scheduleLayout();
}

void GalleryView::showEvent(QShowEvent *)
{
    scheduleLayout();
}

void GalleryView::mouseDoubleClickEvent(QMouseEvent *)
{
    if (scene()->selectedItems().length() > 0) {
        emit enterItem();
    }
}

void GalleryView::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return) {
        // If not item is selected, select the first one
        if (scene()->selectedItems().count() == 0) {
            const QList<QGraphicsItem *> &items =
                scene()->items(Qt::AscendingOrder);
            if (items.count() > 0) {
                items[0]->setSelected(true);
            } else {
                return;
            }
        }

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

    scheduleLayout();
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

void GalleryView::setWaterfallRenderer()
{
    setRendererFactory(new WaterfallRendererFactory());
}

void GalleryView::enableGrouping()
{
    m_enableGrouping = true;
    scheduleLayout();
}

void GalleryView::disableGrouping()
{
    m_enableGrouping = false;
    scheduleLayout();
}

void GalleryView::itemReadyToShow()
{
    m_loadingItemsCount--;
    if (m_loadingItemsCount <= 0) {
        emit loadEnd();
    }
}

void GalleryView::incrItemsToLoad(int count)
{
    m_loadingItemsCount += count;
    if (m_loadingItemsCount > 0) {
        emit loadStart();
    }
}
