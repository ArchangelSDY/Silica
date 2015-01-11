#include "LooseImageRenderer.h"
#include "LooseImageBackgroundRenderer.h"
#include "WaterfallImageRenderer.h"
#include "WaterfallGalleryViewRenderer.h"
#include "WaterfallRendererFactory.h"

AbstractGalleryItemRenderer *WaterfallRendererFactory::createItemRendererForImageGallery()
{
    return new WaterfallImageRenderer();
}

AbstractGalleryItemRenderer *WaterfallRendererFactory::createItemRendererForPlayListGallery(
    const QString &, const int)
{
    return new LooseImageBackgroundRenderer(new LooseImageRenderer());
}

AbstractGalleryItemRenderer *WaterfallRendererFactory::createItemRendererForFileSystemView(
    const QFileInfo &)
{
    return new LooseImageBackgroundRenderer(new LooseImageRenderer());
}

AbstractGalleryViewRenderer *WaterfallRendererFactory::createViewRenderer(
    QGraphicsScene *scene)
{
    return new WaterfallGalleryViewRenderer(scene);
}
