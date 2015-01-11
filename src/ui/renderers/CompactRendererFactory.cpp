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
        QRect bgRect =
            QRect(QPoint(0, 0), GlobalConfig::instance()->galleryItemSize());
        renderer = (new CompactImageRenderer(renderer, 0, bgRect))
            ->translate( -bgRect.width() * 0.1, - bgRect.height() * 0.1);
        renderer->setImage(new QImage(":/res/folder.png"), true);

        // Cover image
        const QSize &gallerySize = GlobalConfig::instance()->galleryItemSize();
        QSize coverSize = gallerySize * 0.65;
        QPoint coverPos = QPoint(
            gallerySize.width() - coverSize.width(),
            gallerySize.height() - CompactTitleRenderer::TITLE_HEIGHT
                - coverSize.height());
        QRect coverRect = QRect(coverPos, coverSize);

        // Image will be set in future so here we give a null
        renderer = (new CompactImageRenderer(renderer, 0, coverRect))
            ->setPadding(3, Qt::white);
    } else {
        renderer = new CompactImageRenderer(renderer);
    }

    // Title
    renderer = new CompactTitleRenderer(pathInfo.fileName(), renderer);

    return renderer;
}
