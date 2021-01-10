#include "LooseImageRenderer.h"
#include "LooseImageBackgroundRenderer.h"
#include "WaterfallImageRenderer.h"
#include "WaterfallGalleryViewRenderer.h"
#include "WaterfallRendererFactory.h"

AbstractGalleryItemRenderer *WaterfallRendererFactory::createItemRendererForImageGallery()
{
    return new WaterfallImageRenderer();
}

AbstractGalleryItemRenderer *WaterfallRendererFactory::createItemRendererForPlayListGallery(
    const QString &, std::optional<int>)
{
    return new LooseImageBackgroundRenderer(new LooseImageRenderer());
}

AbstractGalleryItemRenderer *WaterfallRendererFactory::createItemRendererForFileSystemView(
    const QFileInfo &, bool)
{
    return new LooseImageBackgroundRenderer(new LooseImageRenderer());
}

AbstractGalleryViewRenderer *WaterfallRendererFactory::createViewRenderer(
    GalleryView *galleryView)
{
    return new WaterfallGalleryViewRenderer(galleryView);
}
