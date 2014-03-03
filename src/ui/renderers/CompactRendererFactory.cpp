#include "CompactImageRenderer.h"
#include "CompactRendererFactory.h"

AbstractGalleryItemRenderer *CompactRendererFactory::createForImageGallery()
{
    return new CompactImageRenderer();
}
