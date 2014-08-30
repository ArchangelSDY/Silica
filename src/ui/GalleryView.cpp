#ifdef ENABLE_OPENGL
#include <QGLWidget>
#endif

#include <QGraphicsItem>

#include "AbstractGalleryViewRenderer.h"
#include "CompactRendererFactory.h"
#include "GalleryItem.h"
#include "GalleryView.h"
#include "GlobalConfig.h"
#include "LooseRendererFactory.h"

GalleryView::GalleryView(QWidget *parent) :
    QGraphicsView(parent) ,
    m_scene(new QGraphicsScene) ,
    m_enableGrouping(false)
{
#ifdef ENABLE_OPENGL
    setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
#endif

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

    QList<QGraphicsItem *> items = m_scene->items(Qt::AscendingOrder);

    QStringList itemGroups;
    if (m_enableGrouping) {
        sortByGroup(&items);

        foreach (QGraphicsItem *item, items) {
            itemGroups << groupForItem(item);
        }
    }

    AbstractGalleryViewRenderer *renderer =
        m_rendererFactory->createViewRenderer(m_scene);
    renderer->layout(items, itemGroups, geometry());
    delete renderer;
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
