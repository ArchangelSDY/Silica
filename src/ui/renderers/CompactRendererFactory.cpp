#include <QFileInfo>

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
    const QFileInfo &pathInfo)
{
    AbstractGalleryItemRenderer *renderer = 0;

    if (pathInfo.isDir()) {
        // Background
        renderer = new CompactImageRenderer(renderer);
        renderer->setImage(new QImage(":/res/folder.png"), true);

        // Cover image
        const QSize &gallerySize = GlobalConfig::instance()->galleryItemSize();
        QSize coverSize = gallerySize * 0.55;
        QPoint coverPos =
            QPoint(gallerySize.width() * 0.4, gallerySize.height() * 0.3);
        QRect coverRect = QRect(coverPos, coverSize);

        // Image will be set in future so here we give a null
        renderer = new CompactImageRenderer(renderer, 0, coverRect);
    } else {
        renderer = new CompactImageRenderer(renderer);
    }

    // Title
    renderer = new CompactTitleRenderer(pathInfo.fileName(), renderer);

    return renderer;
}
