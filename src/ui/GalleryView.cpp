#include "ui/GalleryView.h"

#include <QGraphicsItem>
#include <QGraphicsProxyWidget>
#include <QGraphicsView>
#include <QScrollbar>
#include <QVBoxLayout>
#include <QLineEdit>

#include "ui/renderers/AbstractGalleryViewRenderer.h"
#include "ui/renderers/CompactRendererFactory.h"
#include "ui/renderers/LooseRendererFactory.h"
#include "ui/renderers/WaterfallRendererFactory.h"
#include "ui/GalleryItem.h"
#include "GlobalConfig.h"

const int GalleryView::LAYOUT_INTERVAL = 10;

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
    m_scene(new QGraphicsScene) ,
    m_searchBox(new QLineEdit()) ,
    m_enableGrouping(false) ,
    m_layoutNeeded(true) ,
    m_rendererFactory(0)
{
    QLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);
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
    palette.setBrush(QPalette::Background, QColor("#323A44"));
    palette.setBrush(QPalette::Foreground, QColor("#71929E"));
    m_scene->setPalette(palette);
    m_scene->setBackgroundBrush(palette.background());

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

    m_layoutTimer.setSingleShot(false);
    connect(&m_layoutTimer, SIGNAL(timeout()), this, SLOT(layout()));
    m_layoutTimer.start(GalleryView::LAYOUT_INTERVAL);
}

GalleryView::~GalleryView()
{
    m_scene->deleteLater();
    m_view->deleteLater();
    delete m_rendererFactory;
}

QGraphicsScene *GalleryView::scene() const
{
    return m_view->scene();
}

QList<GalleryItem *> GalleryView::galleryItems() const
{
    QList<GalleryItem *> items;
    foreach (QGraphicsItem *rawItem, scene()->items(Qt::AscendingOrder)) {
        GalleryItem *item = dynamic_cast<GalleryItem *>(rawItem);
        if (item) {
            items << item;
        }
    }
    return items;
}

QList<GalleryItem *> GalleryView::selectedGalleryItems() const
{
    QList<GalleryItem *> items;
    foreach (QGraphicsItem *rawItem, scene()->selectedItems()) {
        GalleryItem *item = dynamic_cast<GalleryItem *>(rawItem);
        if (item) {
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
    m_loadProgress.reset();
    m_loadProgress.setMaximum(0);
}

void GalleryView::layout()
{
    if (!m_layoutNeeded || !isVisible()) {
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
            ++iter;
        }
    }

    if (items.count() == 0) {
        return;
    }

    QStringList itemGroups;
    if (m_enableGrouping) {
        sortItemByGroup(&items);

        foreach (GalleryItem *item, items) {
            itemGroups << groupForItem(item);
        }
    }

    AbstractGalleryViewRenderer *renderer =
        m_rendererFactory->createViewRenderer(this);
    renderer->layout(items, itemGroups, geometry());
    delete renderer;

    m_layoutNeeded = false;
    m_layoutTimer.stop();

    markItemsInsideViewportPreload();
}

void GalleryView::scheduleLayout()
{
    m_layoutNeeded = true;
    m_layoutTimer.start();
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
    return m_rendererFactory;
}

void GalleryView::setRendererFactory(AbstractRendererFactory *factory)
{
    if (m_rendererFactory) {
        delete m_rendererFactory;
    }
    m_rendererFactory = factory;

    // Update renderers for each item
    foreach (GalleryItem *item, galleryItems()) {
        item->setRendererFactory(m_rendererFactory);
    }

    scheduleLayout();
    m_view->viewport()->update();
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
    m_loadProgress.setValue(m_loadProgress.value() + 1);
    if (m_loadProgress.value() == m_loadProgress.maximum()) {
        m_loadProgress.stop();
    }
}

void GalleryView::incrItemsToLoad(int count)
{
    m_loadProgress.setMaximum(m_loadProgress.maximum() + count);
    if (!m_loadProgress.isRunning()
            && m_loadProgress.maximum() > m_loadProgress.minimum()) {
        m_loadProgress.start();
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
    QMatrix mat = m_view->matrix().inverted();
    QRectF visibleArea = mat.mapRect(QRectF(tl, br));

    // Enlarge by 9 times
    QRectF preloadArea = QRectF(visibleArea.left() - visibleArea.width(),
                                visibleArea.top() - visibleArea.height(),
                                visibleArea.right() + visibleArea.width(),
                                visibleArea.bottom() + visibleArea.height());

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
