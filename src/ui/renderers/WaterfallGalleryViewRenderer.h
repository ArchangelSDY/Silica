#pragma once

#include "AbstractGalleryViewRenderer.h"

class WaterfallGalleryViewRenderer : public AbstractGalleryViewRenderer
{
public:
    WaterfallGalleryViewRenderer(GalleryView *galleryView, int maxColumns);

    void layout(QList<GalleryItem *> &items,
                const QStringList &itemGroups,
                const QRect &viewGeometry);

private:
    int m_maxColumns;
};