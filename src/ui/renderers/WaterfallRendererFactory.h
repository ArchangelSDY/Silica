#pragma once

#include "AbstractRendererFactory.h"

class WaterfallRendererFactory : public AbstractRendererFactory
{
public:
    WaterfallRendererFactory(int itemWidth);

    virtual AbstractGalleryItemRenderer *createItemRendererForImageGallery() override;
    virtual AbstractGalleryItemRenderer *createItemRendererForPlayListGallery(
        const QString &title, std::optional<int> count) override;
    virtual AbstractGalleryItemRenderer *createItemRendererForFileSystemView(
        const QFileInfo &, bool) override;

    virtual AbstractGalleryViewRenderer *createViewRenderer(
        GalleryView *galleryView) override;

private:
    int m_itemWidth;
};
