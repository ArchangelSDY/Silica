#include <QFileInfo>

#include "CompactCornerIconRenderer.h"
#include "CompactCountRenderer.h"
#include "CompactImageRenderer.h"
#include "CompactRendererFactory.h"
#include "CompactTitleRenderer.h"
#include "GlobalConfig.h"

AbstractGalleryItemRenderer *CompactRendererFactory::createItemRendererForImageGallery()
{
    return new CompactImageRenderer();
}

AbstractGalleryItemRenderer *CompactRendererFactory::createItemRendererForPlayListGallery(
    const QString &title, std::optional<int> count)
{
    auto renderer = new CompactTitleRenderer(title,
        new CompactImageRenderer());
    if (count.has_value()) {
        return new CompactCountRenderer(count.value(), renderer);
    } else {
        return renderer;
    }
}

AbstractGalleryItemRenderer *CompactRendererFactory::createItemRendererForFileSystemView(
    const QFileInfo &pathInfo, bool useDefaultFolderCover)
{
    AbstractGalleryItemRenderer *renderer = new CompactImageRenderer();

    if (pathInfo.isDir() && !useDefaultFolderCover) {
        renderer = new CompactCornerIconRenderer(
            QImage(":/res/folder.png"), renderer);
    }

    // Title
    renderer = new CompactTitleRenderer(pathInfo.fileName(), renderer);

    return renderer;
}
