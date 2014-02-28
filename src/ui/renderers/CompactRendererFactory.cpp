#include "CompactImageRenderer.h"
#include "CompactRendererFactory.h"

AbstractGalleryItemRenderer *CompactRendererFactory::createRenderer()
{
    return new CompactImageRenderer();
}
