#include "LooseImageRenderer.h"
#include "LooseImageBackgroundRenderer.h"
#include "LooseRendererFactory.h"

AbstractGalleryItemRenderer *LooseRendererFactory::createForImageGallery()
{
    return new LooseImageBackgroundRenderer(new LooseImageRenderer());
}
