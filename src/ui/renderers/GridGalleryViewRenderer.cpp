#include <QGraphicsItem>

#include "GalleryItem.h"
#include "GlobalConfig.h"
#include "GridGalleryViewRenderer.h"
#include "ItemGroupTitle.h"

static const qreal GROUP_PADDING_ROW = 0.2;

GridGalleryViewRenderer::GridGalleryViewRenderer(GalleryView *galleryView) :
    AbstractGalleryViewRenderer(galleryView)
{
}

void GridGalleryViewRenderer::layout(QList<GalleryItem *> &items,
        const QStringList &itemGroups, const QRect &viewGeometry)
{
    bool isGroupingEnabled = !itemGroups.isEmpty();

    const QSize &galleryItemSize = GlobalConfig::instance()->galleryItemSize();
    int maxColumns = viewGeometry.width() / galleryItemSize.width();
    if (maxColumns == 0) {
        return;
    }
    qreal sceneWidth = maxColumns * galleryItemSize.width();
    qreal groupTitleHeight = GROUP_PADDING_ROW * galleryItemSize.height();
    qreal curRow = 0, curColumn = -1;

    destroyGroupTitles();

    QString curGroup;
    if (isGroupingEnabled) {
        curGroup = itemGroups[0];
        QRectF titleRect(0, curRow * galleryItemSize.height(),
                         sceneWidth,
                         groupTitleHeight);
        createGroupTitle(itemGroups[0], titleRect);
        curRow += GROUP_PADDING_ROW;
    }

    for (int i = 0; i < items.length(); ++i) {
        GalleryItem *item = items[i];

        // Break line if exceeds columns limit
        if (curColumn == maxColumns - 1) {
            curRow += 1;
            curColumn = 0;
        } else {
            curColumn += 1;
        }

        // Add group title row
        if (isGroupingEnabled) {
            if (itemGroups[i] != curGroup) {
                // Break line first if current item is not at row head
                if (curColumn > 0) {
                    curRow += 1;
                }

                QRectF titleRect(0, curRow * galleryItemSize.height(),
                                 sceneWidth,
                                 groupTitleHeight);
                createGroupTitle(itemGroups[i], titleRect);

                // Move to next item row
                curRow += GROUP_PADDING_ROW;
                curColumn = 0;
            }

            curGroup = itemGroups[i];
        }

        qreal x = curColumn * galleryItemSize.width();
        qreal y = curRow * galleryItemSize.height();
        item->setPos(x, y);

        if (item->isReadyToShow()) {
            item->show();
        }
    }

    QSizeF newSceneSize(sceneWidth,
                        (curRow + 1) * galleryItemSize.height());
    newSceneSize = newSceneSize.expandedTo(QSize(0, viewGeometry.height()));
    QRectF newSceneRect(QPointF(0, 0), newSceneSize);
    scene()->setSceneRect(newSceneRect);
}

void GridGalleryViewRenderer::createGroupTitle(const QString &title,
                                               const QRectF &rect)
{
    ItemGroupTitle *item = new ItemGroupTitle(title);
    item->setPos(rect.topLeft());
    item->setMinSize(rect.size());
    itemGroupTitles() << item;
    scene()->addItem(item);
}

void GridGalleryViewRenderer::destroyGroupTitles()
{
    foreach (QGraphicsItem *title, itemGroupTitles()) {
        scene()->removeItem(title);
        delete title;
    }
    itemGroupTitles().clear();
}
