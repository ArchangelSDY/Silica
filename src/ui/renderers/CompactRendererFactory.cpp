#include "CompactCountRenderer.h"
#include "CompactImageRenderer.h"
#include "CompactRendererFactory.h"
#include "CompactTitleRenderer.h"

AbstractGalleryItemRenderer *CompactRendererFactory::createItemRendererForImageGallery()
{
    return new CompactImageRenderer();
}

AbstractGalleryItemRenderer *CompactRendererFactory::createItemRendererForPlayListGallery(
    const QString &title, const int count)
{
    return new CompactCountRenderer(count,
        new CompactTitleRenderer(title,
        new CompactImageRenderer()));
}

AbstractGalleryItemRenderer *CompactRendererFactory::createItemRendererForFileSystemView(
    const QString &title)
{
    return new CompactTitleRenderer(title, new CompactImageRenderer());
}
