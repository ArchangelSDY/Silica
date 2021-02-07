#include "WaterfallImageRenderer.h"

#include <algorithm>
#include <limits>
#include <QQueue>

#include "GalleryItem.h"
#include "GlobalConfig.h"
#include "WaterfallGalleryViewRenderer.h"

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

static bool tryLayoutMultiColumnItem(GalleryItem *item, int itemWidth, int maxColumns, QList<qreal> &columnHeights, qreal &x, qreal &y)
{
    auto minHeightIter = std::min_element(columnHeights.constBegin(), columnHeights.constEnd());
    int columnIndex = minHeightIter - columnHeights.constBegin();

    if (columnIndex >= 1) {
        // Combine with left column
        int diff = columnHeights[columnIndex - 1] - columnHeights[columnIndex];
        if (abs(diff) < 50) {
            x = ((qreal)columnIndex - 1) * (qreal)itemWidth;
            if (diff > 0) {
                y = columnHeights[columnIndex - 1];
            } else {
                y = columnHeights[columnIndex];
            }
            columnHeights[columnIndex - 1] = y + item->boundingRect().height();
            columnHeights[columnIndex] = y + item->boundingRect().height();
            return true;
        }
    }

    if (columnIndex < maxColumns - 1) {
        // Combine with right column
        int diff = columnHeights[columnIndex] - columnHeights[columnIndex + 1];
        if (abs(diff) < 50) {
            x = (qreal)columnIndex * (qreal)itemWidth;
            if (diff > 0) {
                y = columnHeights[columnIndex];
            } else {
                y = columnHeights[columnIndex + 1];
            }
            columnHeights[columnIndex] = y + item->boundingRect().height();
            columnHeights[columnIndex + 1] = y + item->boundingRect().height();
            return true;
        }
    }

    return false;
}

static void layoutPendingMultiColumnItems(int itemWidth, int maxColumns, QQueue<GalleryItem *> &pendingMultiColumnItems, QList<qreal> &columnHeights)
{
    while (pendingMultiColumnItems.count() > 0) {
        GalleryItem *item = pendingMultiColumnItems.dequeue();

        int columnIndex = 0;
        qreal minConsecutiveHeight = std::numeric_limits<qreal>::max();
        for (int i = 0; i < maxColumns - 1; i++) {
            qreal height = std::max(columnHeights[i], columnHeights[i + 1]);
            if (height < minConsecutiveHeight) {
                minConsecutiveHeight = height;
                columnIndex = i;
            }
        }
        qreal x = (qreal)columnIndex * (qreal)itemWidth;
        qreal y;
        if (columnHeights[columnIndex] > columnHeights[columnIndex + 1]) {
            y = columnHeights[columnIndex];
        } else {
            y = columnHeights[columnIndex + 1];
        }
        columnHeights[columnIndex] = y + item->boundingRect().height();
        columnHeights[columnIndex + 1] = y + item->boundingRect().height();

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
        while (pendingMultiColumnItems.count() > 0) {
            qreal x, y;
            GalleryItem *item = pendingMultiColumnItems.head();
            if (tryLayoutMultiColumnItem(item, galleryItemSize.width(), maxColumns, columnHeights, x, y)) {
                setItemPos(item, x, y);
                pendingMultiColumnItems.dequeue();
            } else {
                break;
            }
        }

        GalleryItem *item = items[i];

        // Group break
        if (isGroupingEnabled) {
            if (itemGroups[i] != curGroup) {
                // Group changed

                // Layout all pending multi items above group break
                layoutPendingMultiColumnItems(galleryItemSize.width(), maxColumns, pendingMultiColumnItems, columnHeights);

                qreal maxHeight = *std::max_element(columnHeights.constBegin(), columnHeights.constEnd());
                for (int i = 0; i < columnHeights.length(); ++i) {
                    columnHeights[i] = maxHeight + WaterfallGalleryViewRenderer::GROUP_MARGIN;
                }
            }

            curGroup = itemGroups[i];
        }

        WaterfallImageRenderer *itemRenderer = static_cast<WaterfallImageRenderer *>(item->renderer());
        if (itemRenderer->columnsSpan() == 1) {
            // Put image in column with lowest height
            auto minHeightIter = std::min_element(columnHeights.constBegin(), columnHeights.constEnd());
            int columnIndex = minHeightIter - columnHeights.constBegin();
            qreal x = (qreal)columnIndex * (qreal)galleryItemSize.width();
            qreal y = columnHeights[columnIndex];
            columnHeights[columnIndex] += item->boundingRect().height();
            setItemPos(item, x, y);
        } else {
            pendingMultiColumnItems.enqueue(item);
        }
    }

    layoutPendingMultiColumnItems(galleryItemSize.width(), maxColumns, pendingMultiColumnItems, columnHeights);

    qreal maxColumnHeight = *std::max_element(columnHeights.constBegin(), columnHeights.constEnd());

    QSizeF newSceneSize((qreal)maxColumns * (qreal)galleryItemSize.width(),
                        maxColumnHeight);
    newSceneSize = newSceneSize.expandedTo(QSize(0, viewGeometry.height()));
    QRectF newSceneRect(QPointF(0, 0), newSceneSize);
    scene()->setSceneRect(newSceneRect);
}
