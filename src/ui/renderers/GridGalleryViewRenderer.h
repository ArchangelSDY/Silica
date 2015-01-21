#ifndef GRIDGALLERYVIEWRENDERER_H
#define GRIDGALLERYVIEWRENDERER_H

#include <QList>

#include "AbstractGalleryViewRenderer.h"

class QGraphicsItem;

class GridGalleryViewRenderer : public AbstractGalleryViewRenderer
{
public:
    GridGalleryViewRenderer(GalleryView *galleryView);

    virtual void layout(QList<GalleryItem *> &items,
                const QStringList &itemGroups,
                const QRect &viewGeometry);

private:
    void createGroupTitle(const QString &title, const QRectF &rect);
    void destroyGroupTitles();
};

#endif // GRIDGALLERYVIEWRENDERER_H
