#include "GlobalConfig.h"
#include "ui/renderers/BoundingRectExpandRenderer.h"
#include "ui/renderers/LooseImageRenderer.h"
#include "ui/renderers/LooseImageBackgroundRenderer.h"
#include "ui/renderers/LooseRendererFactory.h"

AbstractGalleryItemRenderer *LooseRendererFactory::createItemRendererForImageGallery()
{
    return new BoundingRectExpandRenderer(
        QRect(QPoint(0, 0), GlobalConfig::instance()->galleryItemSize()),
        new LooseImageRenderer());
}

AbstractGalleryItemRenderer *LooseRendererFactory::createItemRendererForPlayListGallery(
    const QString &, const int)
{
    return new LooseImageBackgroundRenderer(new LooseImageRenderer());
}

AbstractGalleryItemRenderer *LooseRendererFactory::createItemRendererForFileSystemView(
    const QFileInfo &)
{
    return new LooseImageBackgroundRenderer(new LooseImageRenderer());
}
