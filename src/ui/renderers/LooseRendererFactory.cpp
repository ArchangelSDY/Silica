#include "LooseImageRenderer.h"
#include "LooseRendererFactory.h"

AbstractGalleryItemRenderer *LooseRendererFactory::createRenderer()
{
    return new LooseImageRenderer();
}
