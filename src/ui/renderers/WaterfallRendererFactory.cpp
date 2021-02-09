#include "LooseImageRenderer.h"
#include "LooseImageBackgroundRenderer.h"
#include "WaterfallImageRenderer.h"
#include "WaterfallGalleryViewRenderer.h"
#include "WaterfallRendererFactory.h"

WaterfallRendererFactory::WaterfallRendererFactory(int itemWidth) :
    m_itemWidth(itemWidth)
{
}

AbstractGalleryItemRenderer *WaterfallRendererFactory::createItemRendererForImageGallery()
{
    return new WaterfallImageRenderer(m_itemWidth);
}

AbstractGalleryItemRenderer *WaterfallRendererFactory::createItemRendererForPlayListGallery(
    const QString &, std::optional<int>)
{
    return new LooseImageBackgroundRenderer(new LooseImageRenderer());
}

AbstractGalleryItemRenderer *WaterfallRendererFactory::createItemRendererForFileSystemView(
    const QFileInfo &, bool)
{
    return new LooseImageBackgroundRenderer(new LooseImageRenderer());
}

AbstractGalleryViewRenderer *WaterfallRendererFactory::createViewRenderer(
    GalleryView *galleryView)
{
    return new WaterfallGalleryViewRenderer(galleryView, m_itemWidth);
}
