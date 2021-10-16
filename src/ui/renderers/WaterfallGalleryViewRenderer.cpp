#include "WaterfallImageRenderer.h"

#include <algorithm>
#include <limits>
#include <QQueue>

#include "GalleryItem.h"
#include "WaterfallGalleryViewRenderer.h"

class WaterfallGalleryViewLayoutHelper
{
public:
    WaterfallGalleryViewLayoutHelper(int itemWidth, int maxColumns);

    qreal width() const;
    qreal height() const;
    void tryLayoutItem(GalleryItem *item);
    void tryLayoutPendingItems();
    void forceLayoutPendingItems();
    void addGroupBreak();

private:
    bool tryLayoutItemOnLevelSpan(GalleryItem *item, qreal &x, qreal &y);
    void forceLayoutItemOnLowestSpan(GalleryItem *item, qreal &x, qreal &y);
    void setItemPos(GalleryItem *item, qreal x, qreal y);
    bool isLevel(int minHeight, int maxHeight);

    static const int SPAN_LEVEL_THRESHOLD = 50;
    static const int GROUP_MARGIN = 25;

    int m_itemWidth;
    int m_maxColumns;
    QList<qreal> m_columnHeights;
    QQueue<GalleryItem *> m_pendingItems;
};

WaterfallGalleryViewLayoutHelper::WaterfallGalleryViewLayoutHelper(int _itemWidth, int _maxColumns)
    : m_itemWidth(_itemWidth)
    , m_maxColumns(_maxColumns)
{
    m_columnHeights.reserve(m_maxColumns);
    for (int i = 0; i < m_maxColumns; i++) {
        m_columnHeights << 0;
    }
}

qreal WaterfallGalleryViewLayoutHelper::width() const
{
    return (qreal)m_maxColumns * (qreal)m_itemWidth;
}

qreal WaterfallGalleryViewLayoutHelper::height() const
{
    return *std::max_element(m_columnHeights.constBegin(), m_columnHeights.constEnd());
}

void WaterfallGalleryViewLayoutHelper::setItemPos(GalleryItem *item, qreal x, qreal y)
{
    item->setPos(x, y);
    if (item->isReadyToShow()) {
        item->show();
    }
}

bool WaterfallGalleryViewLayoutHelper::isLevel(int minHeight, int maxHeight)
{
    return maxHeight - minHeight < SPAN_LEVEL_THRESHOLD;
}

void WaterfallGalleryViewLayoutHelper::tryLayoutItem(GalleryItem *item)
{
    qreal x, y;
    if (tryLayoutItemOnLevelSpan(item, x, y)) {
        setItemPos(item, x, y);
    } else {
        m_pendingItems.enqueue(item);
    }
}

bool WaterfallGalleryViewLayoutHelper::tryLayoutItemOnLevelSpan(GalleryItem *item, qreal &x, qreal &y)
{
    WaterfallImageRenderer *renderer = static_cast<WaterfallImageRenderer *>(item->renderer());
    int columnsSpan = renderer->columnsSpan();

    auto minHeightIter = std::min_element(m_columnHeights.constBegin(), m_columnHeights.constEnd());
    int columnIndex = minHeightIter - m_columnHeights.constBegin();

    // Try all possible spans with `columnIndex` inside
    int leftBound = std::max(columnIndex - columnsSpan + 1, 0);
    int rightBound = std::min(columnIndex + columnsSpan - 1, m_maxColumns - 1);

    for (int i = leftBound; i <= rightBound - columnsSpan + 1; i++) {
        // Check if the span is "level". In other words, column height differences are within a certain range
        auto [minIter, maxIter] = std::minmax_element(m_columnHeights.constBegin() + i, m_columnHeights.constBegin() + i + columnsSpan);
        if (isLevel(*minIter, *maxIter)) {
            x = (qreal)i * (qreal)m_itemWidth;
            y = *maxIter;
            for (auto it = m_columnHeights.begin() + i; it != m_columnHeights.begin() + i + columnsSpan; it++) {
                *it = y + item->boundingRect().height();
            }
            return true;
        }
    }

    // If we cannot find any good span, return false and item will be queued
    return false;
}

void WaterfallGalleryViewLayoutHelper::tryLayoutPendingItems()
{
    while (m_pendingItems.count() > 0) {
        qreal x, y;
        GalleryItem *item = m_pendingItems.head();
        if (tryLayoutItemOnLevelSpan(item, x, y)) {
            setItemPos(item, x, y);
            m_pendingItems.dequeue();
        } else {
            return;
        }
    }
}

void WaterfallGalleryViewLayoutHelper::forceLayoutItemOnLowestSpan(GalleryItem *item, qreal &x, qreal &y)
{
    WaterfallImageRenderer *renderer = static_cast<WaterfallImageRenderer *>(item->renderer());
    int columnsSpan = renderer->columnsSpan();

    // Find a span with minimum height
    int columnIndex = 0;
    qreal minHeight = std::numeric_limits<qreal>::max();
    for (int i = 0; i <= m_maxColumns - columnsSpan; i++) {
        // Span height is the max column height within span
        qreal height = *std::max_element(m_columnHeights.constBegin() + i, m_columnHeights.constBegin() + i + columnsSpan);
        if (height < minHeight) {
            minHeight = height;
            columnIndex = i;
        }
    }
    x = (qreal)columnIndex * (qreal)m_itemWidth;
    y = minHeight;
    for (auto it = m_columnHeights.begin() + columnIndex; it != m_columnHeights.begin() + columnIndex + columnsSpan; it++) {
        *it = y + item->boundingRect().height();
    }
}

void WaterfallGalleryViewLayoutHelper::forceLayoutPendingItems()
{
    while (m_pendingItems.count() > 0) {
        GalleryItem *item = m_pendingItems.dequeue();
        qreal x, y;
        forceLayoutItemOnLowestSpan(item, x, y);
        setItemPos(item, x, y);
    }
}

void WaterfallGalleryViewLayoutHelper::addGroupBreak()
{
    // Layout all pending multi items above group break
    forceLayoutPendingItems();

    qreal h = height();
    for (int i = 0; i < m_columnHeights.length(); ++i) {
        m_columnHeights[i] = h + GROUP_MARGIN;
    }
}


WaterfallGalleryViewRenderer::WaterfallGalleryViewRenderer(
    GalleryView *galleryView, int itemWidth) :
    AbstractGalleryViewRenderer(galleryView) ,
    m_itemWidth(itemWidth)
{
}

void WaterfallGalleryViewRenderer::layout(
        QList<GalleryItem *> &items, const QStringList &itemGroups,
        const QRect &viewGeometry)
{
    bool isGroupingEnabled = !itemGroups.isEmpty();

    int maxColumns = viewGeometry.width() / m_itemWidth;
    if (maxColumns <= 1) {
        return;
    }

    WaterfallGalleryViewLayoutHelper helper(m_itemWidth, maxColumns);

    QString curGroup;
    if (isGroupingEnabled) {
        curGroup = itemGroups[0];
    }

    for (int i = 0; i < items.length(); ++i) {
        helper.tryLayoutPendingItems();

        GalleryItem *item = items[i];

        // Group break
        if (isGroupingEnabled) {
            if (itemGroups[i] != curGroup) {
                // Group changed
                helper.addGroupBreak();
            }
            curGroup = itemGroups[i];
        }

        WaterfallImageRenderer *itemRenderer = static_cast<WaterfallImageRenderer *>(item->renderer());
        helper.tryLayoutItem(item);
    }

    helper.tryLayoutPendingItems();
    helper.forceLayoutPendingItems();

    QSizeF newSceneSize(helper.width(), helper.height());
    newSceneSize = newSceneSize.expandedTo(QSize(0, viewGeometry.height()));
    QRectF newSceneRect(QPointF(0, 0), newSceneSize);
    scene()->setSceneRect(newSceneRect);
}
