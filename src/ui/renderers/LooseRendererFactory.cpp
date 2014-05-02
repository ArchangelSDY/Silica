#include "LooseImageRenderer.h"
#include "LooseImageBackgroundRenderer.h"
#include "LooseRendererFactory.h"

AbstractGalleryItemRenderer *LooseRendererFactory::createForImageGallery()
{
    return new LooseImageRenderer();
}

AbstractGalleryItemRenderer *LooseRendererFactory::createForPlayListGallery(
    const QString &, const int)
{
    return new LooseImageBackgroundRenderer(new LooseImageRenderer());
}
