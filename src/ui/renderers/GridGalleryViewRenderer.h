#ifndef GRIDGALLERYVIEWRENDERER_H
#define GRIDGALLERYVIEWRENDERER_H

#include "AbstractGalleryViewRenderer.h"

class GridGalleryViewRenderer : public AbstractGalleryViewRenderer
{
public:
    GridGalleryViewRenderer(QGraphicsScene *scene);

    virtual void layout(QList<QGraphicsItem *> &items,
                const QStringList &itemGroups,
                const QRect &viewGeometry);
};

#endif // GRIDGALLERYVIEWRENDERER_H
