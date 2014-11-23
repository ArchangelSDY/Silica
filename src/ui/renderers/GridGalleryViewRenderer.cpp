#include "GalleryItem.h"
#include "GlobalConfig.h"
#include "GridGalleryViewRenderer.h"

static const qreal GROUP_PADDING_ROW = 0.1;

GridGalleryViewRenderer::GridGalleryViewRenderer(QGraphicsScene *scene) :
    AbstractGalleryViewRenderer(scene)
{
}

void GridGalleryViewRenderer::layout(QList<QGraphicsItem *> &items,
        const QStringList &itemGroups, const QRect &viewGeometry)
{
    bool isGroupingEnabled = !itemGroups.isEmpty();

    const QSize &galleryItemSize = GlobalConfig::instance()->galleryItemSize();
    int maxColumns = viewGeometry.width() / galleryItemSize.width();
    if (maxColumns == 0) {
        return;
    }

    qreal curRow = 0, curColumn = -1;

    QString curGroup;
    if (isGroupingEnabled) {
        curGroup = itemGroups[0];
    }

    for (int i = 0; i < items.length(); ++i) {
        GalleryItem *item = static_cast<GalleryItem *>(items[i]);

        bool shouldBreakLine = false;
        // Break line if exceeds columns limit
        if (curColumn == maxColumns - 1) {
            shouldBreakLine = true;
        }

        // Break line if group changs
        if (isGroupingEnabled) {
            if (itemGroups[i] != curGroup) {
                curRow += GROUP_PADDING_ROW;    // Add additional padding row

                shouldBreakLine = true;
            }

            curGroup = itemGroups[i];
        }

        if (shouldBreakLine) {
            curRow += 1;
            curColumn = 0;
        } else {
            curColumn += 1;
        }

        qreal x = curColumn * galleryItemSize.width();
        qreal y = curRow * galleryItemSize.height();
        item->setPos(x, y);

        if (item->isReadyToShow()) {
            item->show();
        }
    }

    QRectF newSceneRect(0, 0,
        maxColumns * galleryItemSize.width(),
        (curRow + 1) * galleryItemSize.height());
    m_scene->setSceneRect(newSceneRect);
}