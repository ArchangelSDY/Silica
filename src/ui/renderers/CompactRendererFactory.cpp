#include "CompactImageRenderer.h"
#include "CompactRendererFactory.h"
#include "CompactTitleRenderer.h"

AbstractGalleryItemRenderer *CompactRendererFactory::createForImageGallery()
{
    return new CompactImageRenderer();
}

AbstractGalleryItemRenderer *CompactRendererFactory::createForPlayListGallery(
    const QString &title)
{
    return new CompactTitleRenderer(title, new CompactImageRenderer());
}
