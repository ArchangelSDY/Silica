#ifndef WATERFALLGALLERYVIEWRENDERER_H
#define WATERFALLGALLERYVIEWRENDERER_H

#include "AbstractGalleryViewRenderer.h"

class WaterfallGalleryViewRenderer : public AbstractGalleryViewRenderer
{
public:
    WaterfallGalleryViewRenderer(GalleryView *galleryView);

    void layout(QList<GalleryItem *> &items,
                const QStringList &itemGroups,
                const QRect &viewGeometry);

private:
    static const int GROUP_MARGIN = 25;
};

#endif // WATERFALLGALLERYVIEWRENDERER_H
