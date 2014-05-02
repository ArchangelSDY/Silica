#include "CompactCountRenderer.h"
#include "CompactImageRenderer.h"
#include "CompactRendererFactory.h"
#include "CompactTitleRenderer.h"

AbstractGalleryItemRenderer *CompactRendererFactory::createForImageGallery()
{
    return new CompactImageRenderer();
}

AbstractGalleryItemRenderer *CompactRendererFactory::createForPlayListGallery(
    const QString &title, const int count)
{
    return new CompactCountRenderer(count,
        new CompactTitleRenderer(title,
        new CompactImageRenderer()));
}
