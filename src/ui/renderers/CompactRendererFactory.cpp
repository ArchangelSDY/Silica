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
    const QString &title, const int count)
{
    return new CompactCountRenderer(count,
        new CompactTitleRenderer(title,
        new CompactImageRenderer()));
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
