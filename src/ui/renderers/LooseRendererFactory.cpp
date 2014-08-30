#include "LooseImageRenderer.h"
#include "LooseImageBackgroundRenderer.h"
#include "LooseRendererFactory.h"

AbstractGalleryItemRenderer *LooseRendererFactory::createItemRendererForImageGallery()
{
    return new LooseImageRenderer();
}

AbstractGalleryItemRenderer *LooseRendererFactory::createItemRendererForPlayListGallery(
    const QString &, const int)
{
    return new LooseImageBackgroundRenderer(new LooseImageRenderer());
}
