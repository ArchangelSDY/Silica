#include "ui/GalleryView.h"

#include <QGraphicsItem>
#include <QGraphicsProxyWidget>
#include <QGraphicsView>
#include <QScrollbar>
#include <QVBoxLayout>
#include <QLineEdit>

#ifdef ENABLE_OPENGL
#include <QOpenGLWidget>
#endif

#include "ui/renderers/AbstractGalleryViewRenderer.h"
#include "ui/renderers/CompactRendererFactory.h"
#include "ui/renderers/LooseRendererFactory.h"
#include "ui/renderers/WaterfallRendererFactory.h"
#include "ui/GalleryItem.h"
#include "GlobalConfig.h"

const int GalleryView::LAYOUT_INTERVAL = 50;

class GalleryView::GraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    GraphicsView(GalleryView *galleryView) : m_galleryView(galleryView)
    {
    }

protected:
    virtual void scrollContentsBy(int dx, int dy) override
    {
        m_galleryView->markItemsInsideViewportPreload();
        QGraphicsView::scrollContentsBy(dx, dy);
    }

private:
    GalleryView *m_galleryView;
};

GalleryView::GalleryView(QWidget *parent) :
    QWidget(parent) ,
    m_view(new GraphicsView(this)) ,
    m_scene(new QGraphicsScene(m_view)) ,
    m_searchBox(new QLineEdit()) ,
    m_enableGrouping(false)
{
#ifdef ENABLE_OPENGL
    auto glWidget = new QOpenGLWidget(this);
    QSurfaceFormat surfaceFormat;
    surfaceFormat.setSamples(16);
    glWidget->setFormat(surfaceFormat);
    m_view->setViewport(glWidget);
#endif

    QLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_view);
    layout->addWidget(m_searchBox);
    setLayout(layout);

    m_view->setDragMode(QGraphicsView::RubberBandDrag);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setScene(m_scene);

    QPalette palette;
    palette.setBrush(QPalette::Window, QColor("#323A44"));
    palette.setBrush(QPalette::WindowText, QColor("#71929E"));
    m_scene->setPalette(palette);
    m_scene->setBackgroundBrush(palette.window());

    // Disable index to speed up adding items
    m_scene->setItemIndexMethod(QGraphicsScene::NoIndex);

    m_searchBox->setStyleSheet(
        "border: none;"
        "padding: 0 0.5em;"
        "min-width: 15em;"
        "max-height: 1.5em");
    m_searchBox->setAttribute(Qt::WA_MacShowFocusRect, false);
    m_searchBox->hide();
    connect(m_searchBox, SIGNAL(textEdited(QString)),
            this, SLOT(setNameFilter(QString)));

    m_layoutTimer.setSingleShot(true);
    connect(&m_layoutTimer, SIGNAL(timeout()), this, SLOT(layout()));
}

QGraphicsScene *GalleryView::scene() const
{
    return m_view->scene();
}

const QList<GalleryItem *> &GalleryView::galleryItems() const
{
    return m_galleryItems;
}

QList<GalleryItem *> GalleryView::selectedGalleryItems() const
{
    QList<GalleryItem *> items;
    for (GalleryItem *item : galleryItems()) {
        if (item->isSelected()) {
            items << item;
        }
    }
    return items;
}

const TaskProgress &GalleryView::loadProgress() const
{
    return m_loadProgress;
}

const TaskProgress &GalleryView::groupingProgress() const
{
    return m_groupingProgress;
}

void GalleryView::clear()
{
    // Clear first
    m_viewportPreloadItems.clear();
    foreach (GalleryItem *item, galleryItems()) {
        m_scene->removeItem(item);
        item->deleteLater();
    }
    m_galleryItems.clear();
    m_loadProgress.reset();
    m_loadProgress.setMaximum(0);
}

void GalleryView::layout()
{
    if (!isVisible()) {
        return;
    }

    QList<GalleryItem *> items = galleryItems();
    if (items.count() == 0) {
        return;
    }

    // Remove and hide items that does not match name filter
    QList<GalleryItem *>::iterator iter = items.begin();
    while (iter != items.end()) {
        GalleryItem *item = *iter;
        QVariant isNameFiltered = item->data(GalleryItem::KEY_IS_NAME_FILTERED);
        if (!isNameFiltered.isNull() && !isNameFiltered.toBool()) {
            item->hide();
            iter = items.erase(iter);
        } else {
            auto itemRenderer = item->renderer();
            itemRenderer->layout(m_scene->sceneRect().toRect());
            ++iter;
        }
    }

    if (items.count() == 0) {
        return;
    }

    QStringList itemGroups;
    if (m_enableGrouping) {
        sortItemByGroup(&items);

        for (auto item : items) {
            itemGroups << groupForItem(item);
        }
    }

    QScopedPointer<AbstractGalleryViewRenderer> renderer(m_rendererFactory->createViewRenderer(this));
    renderer->layout(items, itemGroups, geometry());

    markItemsInsideViewportPreload();
}

void GalleryView::scheduleLayout()
{
    if (!m_layoutTimer.isActive()) {
        m_layoutTimer.start(LAYOUT_INTERVAL);
    }
}

void GalleryView::setNameFilter(const QString &nameFilter)
{
    m_nameFilter = nameFilter;
    m_searchBox->setText(m_nameFilter);
    foreach (GalleryItem *item, galleryItems()) {
        markItemIsFiltered(item);
    }
    scheduleLayout();
}

void GalleryView::resizeEvent(QResizeEvent *)
{
    m_scene->setSceneRect(rect());
    scheduleLayout();
}

void GalleryView::showEvent(QShowEvent *)
{
    scheduleLayout();
}

void GalleryView::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return) {
        // If not item is selected, select the first one
        if (selectedGalleryItems().count() == 0) {
            QList<GalleryItem *> items = galleryItems();
            if (items.count() > 0) {
                items[0]->setSelected(true);
            } else {
                return;
            }
        }

        event->accept();
        emit keyEnterPressed();
    } else if (event->key() == Qt::Key_Escape) {
        scene()->clearSelection();
        event->accept();
    } else if (event->key() == Qt::Key_A && event->modifiers().testFlag(Qt::ControlModifier)) {
        QPainterPath path;
        path.addRect(scene()->sceneRect());
        scene()->setSelectionArea(path);
        event->accept();
    } else if (event->key() == Qt::Key_Slash) {
        enterSearch();
        event->accept();
    } else if (event->key() == Qt::Key_Escape && m_searchBox->isVisible()) {
        leaveSearch();
        event->accept();
    } else {
        QWidget::keyPressEvent(event);
    }
}

void GalleryView::itemMouseDoubleClicked()
{
    if (selectedGalleryItems().length() > 0) {
        emit mouseDoubleClicked();
    }
}

void GalleryView::addItem(GalleryItem *item)
{
    m_galleryItems << item;
    setupItem(item);
}

void GalleryView::replaceItem(int index, GalleryItem *item)
{
    GalleryItem *oldItem = m_galleryItems[index];

    // Keep states
    item->setSelected(oldItem->isSelected());
    item->setPos(oldItem->pos());

    // Remove old item
    m_scene->removeItem(oldItem);
    oldItem->deleteLater();
    m_viewportPreloadItems.remove(oldItem);

    // Replace with new item
    m_galleryItems.replace(index, item);
    setupItem(item);
}

void GalleryView::setupItem(GalleryItem *item)
{
    m_scene->addItem(item);
    markItemIsFiltered(item);
    connect(item, SIGNAL(requestLayout()), this, SLOT(scheduleLayout()),
            Qt::UniqueConnection);
    connect(item, SIGNAL(mouseDoubleClicked()),
            this, SLOT(itemMouseDoubleClicked()), Qt::UniqueConnection);
    connect(item, SIGNAL(readyToShow()), this, SLOT(itemReadyToShow()));

    // Set it in preload state to trigger a load
    // TODO: Try to refactor this to make more sense
    item->setIsInsideViewportPreload(true);
    m_viewportPreloadItems << item;
}

AbstractRendererFactory *GalleryView::rendererFactory()
{
    return m_rendererFactory.data();
}

void GalleryView::setRendererFactory(AbstractRendererFactory *factory)
{
    m_rendererFactory.reset(factory);

    // Update renderers for each item
    foreach (GalleryItem *item, galleryItems()) {
        item->setRendererFactory(m_rendererFactory.data());
    }

    scheduleLayout();
    m_view->viewport()->update();
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
    m_loadProgress.setValue(m_loadProgress.value() + 1);
    if (m_loadProgress.value() == m_loadProgress.maximum()) {
        m_loadProgress.stop();
    }
}

void GalleryView::incrItemsToLoad(int count)
{
    if (count > 0) {
        m_loadProgress.setMaximum(m_loadProgress.maximum() + count);
        if (!m_loadProgress.isRunning()
                && m_loadProgress.maximum() > m_loadProgress.minimum()) {
            m_loadProgress.start();
        }
    }
}

void GalleryView::markItemIsFiltered(GalleryItem *item)
{
    bool isFiltered = item->name().contains(m_nameFilter, Qt::CaseInsensitive);
    item->setData(GalleryItem::KEY_IS_NAME_FILTERED, isFiltered);
}

void GalleryView::markItemsInsideViewportPreload()
{
    // Compute visible area
    QPointF tl(m_view->horizontalScrollBar()->value(), m_view->verticalScrollBar()->value());
    QPointF br = tl + m_view->viewport()->rect().bottomRight();
    QTransform mat = m_view->transform().inverted();
    QRectF visibleArea = mat.mapRect(QRectF(tl, br));

    // Enlarge by 25 times
    QRectF preloadArea = QRectF(visibleArea.left() - visibleArea.width() * 2,
                                visibleArea.top() - visibleArea.height() * 2,
                                visibleArea.width() * 5,
                                visibleArea.height() * 5);

    QList<QGraphicsItem *> newPreloadItemsList = m_scene->items(preloadArea);
    QSet<QGraphicsItem *> newPreloadItems;
    for (QGraphicsItem *item : newPreloadItemsList) {
        if (dynamic_cast<GalleryItem *>(item) != nullptr) {
            newPreloadItems << item;
        }
    }

    for (QGraphicsItem *item : newPreloadItems) {
        if (!m_viewportPreloadItems.contains(item)) {
            GalleryItem *galleryItem = static_cast<GalleryItem *>(item);
            galleryItem->setIsInsideViewportPreload(true);
        }
    }

    for (QGraphicsItem *item : m_viewportPreloadItems) {
        if (!newPreloadItems.contains(item)) {
            GalleryItem *galleryItem = static_cast<GalleryItem *>(item);
            galleryItem->setIsInsideViewportPreload(false);
        }
    }

    m_viewportPreloadItems = newPreloadItems;
}

void GalleryView::enterSearch()
{
    QRect pos(QPoint(geometry().width() - m_searchBox->width(), 0),
              m_searchBox->size());
    m_searchBox->setGeometry(pos);
    m_searchBox->show();
    m_searchBox->setFocus();
}

void GalleryView::leaveSearch()
{
    m_searchBox->hide();
    setNameFilter(QString());
    setFocus();
}

#include "GalleryView.moc"
