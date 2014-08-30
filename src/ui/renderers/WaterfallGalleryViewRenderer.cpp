#include "GalleryItem.h"
#include "GlobalConfig.h"
#include "WaterfallGalleryViewRenderer.h"

WaterfallGalleryViewRenderer::WaterfallGalleryViewRenderer(
        QGraphicsScene *scene) :
    AbstractGalleryViewRenderer(scene)
{
}

void WaterfallGalleryViewRenderer::layout(
        QList<QGraphicsItem *> &items, const QStringList &itemGroups,
        const QRect &viewGeometry)
{
    bool isGroupingEnabled = !itemGroups.isEmpty();

    const QSize &galleryItemSize = GlobalConfig::instance()->galleryItemSize();
    int maxColumns = viewGeometry.width() / galleryItemSize.width();
    if (maxColumns == 0) {
        return;
    }

    QList<qreal> columnHeights;
    for (int i = 0; i < maxColumns; ++i) {
        columnHeights << 0;
    }

    QString curGroup;
    if (isGroupingEnabled) {
        curGroup = itemGroups[0];
    }

    for (int i = 0; i < items.length(); ++i) {
        GalleryItem *item = static_cast<GalleryItem *>(items[i]);

        // Group break
        if (isGroupingEnabled) {
            if (itemGroups[i] != curGroup) {
                qreal newY = columnHeights[0];
                foreach (const qreal &y, columnHeights) {
                    newY = qMax(newY, y);
                }

                for (int i = 0; i < columnHeights.length(); ++i) {
                    columnHeights[i] =
                        newY + WaterfallGalleryViewRenderer::GROUP_MARGIN;
                }
            }

            curGroup = itemGroups[i];
        }

        int columnIndex = i % maxColumns;
        qreal x = columnIndex * galleryItemSize.width();
        qreal y = columnHeights[columnIndex];

        item->setPos(x, y);

        columnHeights[columnIndex] += item->boundingRect().height();
    }

    qreal maxColumnHeight = columnHeights[0];
    foreach (const qreal &y, columnHeights) {
        maxColumnHeight = qMax(maxColumnHeight, y);
    }

    QRectF newSceneRect(0, 0,
        maxColumns * galleryItemSize.width(),
        maxColumnHeight);
    m_scene->setSceneRect(newSceneRect);
}
