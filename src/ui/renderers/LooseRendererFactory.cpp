#include "LooseImageRenderer.h"
#include "LooseImageBackgroundRenderer.h"
#include "LooseRendererFactory.h"

AbstractGalleryItemRenderer *LooseRendererFactory::createForImageGallery()
{
    return new LooseImageRenderer();
}

AbstractGalleryItemRenderer *LooseRendererFactory::createForPlayListGallery(
    const QString &)
{
    return new LooseImageBackgroundRenderer(new LooseImageRenderer());
}
