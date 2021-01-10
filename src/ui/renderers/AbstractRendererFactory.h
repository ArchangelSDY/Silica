#ifndef ABSTRACTRENDERERFACTORY_H
#define ABSTRACTRENDERERFACTORY_H

#include <optional>

#include <QGraphicsScene>
#include <QString>

class QFileInfo;

class AbstractGalleryItemRenderer;
class AbstractGalleryViewRenderer;
class GalleryView;

class AbstractRendererFactory
{
public:
    virtual ~AbstractRendererFactory() {}

    virtual AbstractGalleryItemRenderer *createItemRendererForImageGallery() = 0;
    virtual AbstractGalleryItemRenderer *createItemRendererForPlayListGallery(
        const QString &title, std::optional<int> count) = 0;
    virtual AbstractGalleryItemRenderer *createItemRendererForFileSystemView(
        const QFileInfo &pathInfo, bool useDefaultFolderCover) = 0;

    virtual AbstractGalleryViewRenderer *createViewRenderer(
        GalleryView *galleryView);
};

#endif // ABSTRACTRENDERERFACTORY_H
