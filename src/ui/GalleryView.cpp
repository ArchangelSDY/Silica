#include <QGraphicsItem>
#include <QLineEdit>

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
    m_searchBox(new QLineEdit(this)) ,
    m_enableGrouping(false) ,
    m_layoutNeeded(true) ,
    m_loadingItemsCount(0) ,
    m_rendererFactory(0)
{
    setDragMode(QGraphicsView::RubberBandDrag);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QPalette palette;
    palette.setBrush(QPalette::Background, QColor("#323A44"));
    palette.setBrush(QPalette::Foreground, QColor("#71929E"));
    m_scene->setPalette(palette);
    m_scene->setBackgroundBrush(palette.background());

    m_searchBox->setStyleSheet(
        "border: none;"
        "padding: 0 0.5em;"
        "min-width: 15em;"
        "max-height: 1.5em");
    m_searchBox->setAttribute(Qt::WA_MacShowFocusRect, false);
    m_searchBox->hide();
    connect(m_searchBox, SIGNAL(textEdited(QString)),
            this, SLOT(setNameFilter(QString)));

    setScene(m_scene);

    m_layoutTimer.setSingleShot(false);
    connect(&m_layoutTimer, SIGNAL(timeout()), this, SLOT(layout()));
    m_layoutTimer.start(GalleryView::LAYOUT_INTERVAL);
}

GalleryView::~GalleryView()
{
    delete m_scene;
    delete m_rendererFactory;
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

void GalleryView::clear()
{
    // Clear first
    foreach (GalleryItem *item, galleryItems()) {
        m_scene->removeItem(item);
        item->deleteLater();
    }
    m_loadingItemsCount = 0;
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

void GalleryView::mouseDoubleClickEvent(QMouseEvent *)
{
    if (selectedGalleryItems().length() > 0) {
        emit mouseDoubleClicked();
    }
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
    } else if (event->key() == Qt::Key_Slash && !m_searchBox->isVisible()) {
        enterSearch();
        event->accept();
    } else if (event->key() == Qt::Key_Escape && m_searchBox->isVisible()) {
        leaveSearch();
        event->accept();
    } else {
        QGraphicsView::keyPressEvent(event);
    }
}

void GalleryView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        event->accept();
        return;
    }

    QGraphicsView::mousePressEvent(event);
}

void GalleryView::addItem(GalleryItem *item)
{
    m_scene->addItem(item);
    markItemIsFiltered(item);
    connect(item, SIGNAL(readyToShow()), this, SLOT(itemReadyToShow()));
    item->load();
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

void GalleryView::markItemIsFiltered(GalleryItem *item)
{
    bool isFiltered = item->name().contains(m_nameFilter, Qt::CaseInsensitive);
    item->setData(GalleryItem::KEY_IS_NAME_FILTERED, isFiltered);
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
