#include "WaterfallImageRenderer.h"

#include <algorithm>
#include <limits>
#include <QQueue>

#include "GalleryItem.h"
#include "GlobalConfig.h"
#include "WaterfallGalleryViewRenderer.h"

static const int SPAN_LEVEL_THRESHOLD = 50;

WaterfallGalleryViewRenderer::WaterfallGalleryViewRenderer(
    GalleryView *galleryView) :
    AbstractGalleryViewRenderer(galleryView)
{
}

static void setItemPos(GalleryItem *item, qreal x, qreal y)
{
    item->setPos(x, y);
    if (item->isReadyToShow()) {
        item->show();
    }
}

static bool isLevel(int minHeight, int maxHeight)
{
    return maxHeight - minHeight < SPAN_LEVEL_THRESHOLD;
}

static bool tryLayoutItemOnLevelSpan(int itemWidth, int maxColumns, QList<qreal> &columnHeights, GalleryItem *item, qreal &x, qreal &y)
{
    WaterfallImageRenderer *renderer = static_cast<WaterfallImageRenderer *>(item->renderer());
    int columnsSpan = renderer->columnsSpan();

    auto minHeightIter = std::min_element(columnHeights.constBegin(), columnHeights.constEnd());
    int columnIndex = minHeightIter - columnHeights.constBegin();

    // Try all possible spans with `columnIndex` inside
    int leftBound = std::max(columnIndex - columnsSpan + 1, 0);
    int rightBound = std::min(columnIndex + columnsSpan - 1, maxColumns - 1);

    for (int i = leftBound; i <= rightBound - columnsSpan + 1; i++) {
        // Check if the span is "level". In other words, column height differences are within a certain range
        auto [minIter, maxIter] = std::minmax_element(columnHeights.constBegin() + i, columnHeights.constBegin() + i + columnsSpan);
        if (isLevel(*minIter, *maxIter)) {
            x = (qreal)i * (qreal)itemWidth;
            y = *maxIter;
            for (auto it = columnHeights.begin() + i; it != columnHeights.begin() + i + columnsSpan; it++) {
                *it = y + item->boundingRect().height();
            }
            return true;
        }
    }

    // If we cannot find any good span, return false and item will be queued
    return false;
}

static void tryLayoutPendingMultiColumnItems(int itemWidth, int maxColumns, QList<qreal> &columnHeights, QQueue<GalleryItem *> &pendingMultiColumnItems)
{
    while (pendingMultiColumnItems.count() > 0) {
        qreal x, y;
        GalleryItem *item = pendingMultiColumnItems.head();
        if (tryLayoutItemOnLevelSpan(itemWidth, maxColumns, columnHeights, item, x, y)) {
            setItemPos(item, x, y);
            pendingMultiColumnItems.dequeue();
        } else {
            return;
        }
    }
}

static void forceLayoutItemOnLowestSpan(int itemWidth, int maxColumns, QList<qreal> &columnHeights, GalleryItem *item, qreal &x, qreal &y)
{
    WaterfallImageRenderer *renderer = static_cast<WaterfallImageRenderer *>(item->renderer());
    int columnsSpan = renderer->columnsSpan();

    // Find a span with minimum height
    int columnIndex = 0;
    qreal minHeight = std::numeric_limits<qreal>::max();
    for (int i = 0; i <= maxColumns - columnsSpan; i++) {
        // Span height is the max column height within span
        qreal height = *std::max_element(columnHeights.constBegin() + i, columnHeights.constBegin() + i + columnsSpan);
        if (height < minHeight) {
            minHeight = height;
            columnIndex = i;
        }
    }
    x = (qreal)columnIndex * (qreal)itemWidth;
    y = minHeight;
    for (auto &it = columnHeights.begin() + columnIndex; it != columnHeights.begin() + columnIndex + columnsSpan; it++) {
        *it = y + item->boundingRect().height();
    }
}

static void forceLayoutPendingMultiColumnItems(int itemWidth, int maxColumns, QList<qreal> &columnHeights, QQueue<GalleryItem *> &pendingMultiColumnItems)
{
    while (pendingMultiColumnItems.count() > 0) {
        GalleryItem *item = pendingMultiColumnItems.dequeue();
        qreal x, y;
        forceLayoutItemOnLowestSpan(itemWidth, maxColumns, columnHeights, item, x, y);
        setItemPos(item, x, y);
    }
}

void WaterfallGalleryViewRenderer::layout(
        QList<GalleryItem *> &items, const QStringList &itemGroups,
        const QRect &viewGeometry)
{
    bool isGroupingEnabled = !itemGroups.isEmpty();

    const QSize &galleryItemSize = GlobalConfig::instance()->galleryItemSize();
    int maxColumns = viewGeometry.width() / galleryItemSize.width();
    if (maxColumns <= 1) {
        return;
    }

    QList<qreal> columnHeights;
    QQueue<GalleryItem *> pendingMultiColumnItems;
    for (int i = 0; i < maxColumns; ++i) {
        columnHeights << 0;
    }

    QString curGroup;
    if (isGroupingEnabled) {
        curGroup = itemGroups[0];
    }

    for (int i = 0; i < items.length(); ++i) {
        tryLayoutPendingMultiColumnItems(galleryItemSize.width(), maxColumns, columnHeights, pendingMultiColumnItems);

        GalleryItem *item = items[i];

        // Group break
        if (isGroupingEnabled) {
            if (itemGroups[i] != curGroup) {
                // Group changed

                // Layout all pending multi items above group break
                forceLayoutPendingMultiColumnItems(galleryItemSize.width(), maxColumns, columnHeights, pendingMultiColumnItems);

                qreal maxHeight = *std::max_element(columnHeights.constBegin(), columnHeights.constEnd());
                for (int i = 0; i < columnHeights.length(); ++i) {
                    columnHeights[i] = maxHeight + WaterfallGalleryViewRenderer::GROUP_MARGIN;
                }
            }

            curGroup = itemGroups[i];
        }

        WaterfallImageRenderer *itemRenderer = static_cast<WaterfallImageRenderer *>(item->renderer());
        qreal x, y;
        if (tryLayoutItemOnLevelSpan(galleryItemSize.width(), maxColumns, columnHeights, item, x, y)) {
            setItemPos(item, x, y);
        } else {
            pendingMultiColumnItems.enqueue(item);
        }
    }

    tryLayoutPendingMultiColumnItems(galleryItemSize.width(), maxColumns, columnHeights, pendingMultiColumnItems);
    forceLayoutPendingMultiColumnItems(galleryItemSize.width(), maxColumns, columnHeights, pendingMultiColumnItems);

    qreal maxColumnHeight = *std::max_element(columnHeights.constBegin(), columnHeights.constEnd());

    QSizeF newSceneSize((qreal)maxColumns * (qreal)galleryItemSize.width(),
                        maxColumnHeight);
    newSceneSize = newSceneSize.expandedTo(QSize(0, viewGeometry.height()));
    QRectF newSceneRect(QPointF(0, 0), newSceneSize);
    scene()->setSceneRect(newSceneRect);
}
