#include "GalleryItem.h"
#include "GlobalConfig.h"
#include "WaterfallGalleryViewRenderer.h"

WaterfallGalleryViewRenderer::WaterfallGalleryViewRenderer(
        QGraphicsScene *scene) :
    AbstractGalleryViewRenderer(scene)
{
}

void WaterfallGalleryViewRenderer::layout(
        QList<GalleryItem *> &items, const QStringList &itemGroups,
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
        GalleryItem *item = items[i];

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

        // Put image in column with lowest height
        qreal minHeight = columnHeights[0];
        int columnIndex = 0;
        for (int i = 1; i < columnHeights.length(); ++ i) {
            qreal y = columnHeights[i];
            if (y < minHeight) {
                columnIndex = i;
                minHeight = y;
            }
        }

        qreal x = columnIndex * galleryItemSize.width();
        qreal y = columnHeights[columnIndex];

        item->setPos(x, y);

        if (item->isReadyToShow()) {
            item->show();
        }

        columnHeights[columnIndex] += item->boundingRect().height();
    }

    qreal maxColumnHeight = columnHeights[0];
    foreach (const qreal &y, columnHeights) {
        maxColumnHeight = qMax(maxColumnHeight, y);
    }

    QSizeF newSceneSize(maxColumns * galleryItemSize.width(),
                        maxColumnHeight);
    newSceneSize = newSceneSize.expandedTo(QSize(0, viewGeometry.height()));
    QRectF newSceneRect(QPointF(0, 0), newSceneSize);
    m_scene->setSceneRect(newSceneRect);
}
