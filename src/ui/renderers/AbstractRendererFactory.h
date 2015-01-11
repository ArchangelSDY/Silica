#ifndef ABSTRACTRENDERERFACTORY_H
#define ABSTRACTRENDERERFACTORY_H

#include <QGraphicsScene>
#include <QString>

class QFileInfo;

class AbstractGalleryItemRenderer;
class AbstractGalleryViewRenderer;

class AbstractRendererFactory
{
public:
    virtual ~AbstractRendererFactory() {}

    virtual AbstractGalleryItemRenderer *createItemRendererForImageGallery() = 0;
    virtual AbstractGalleryItemRenderer *createItemRendererForPlayListGallery(
        const QString &title, const int count) = 0;
    virtual AbstractGalleryItemRenderer *createItemRendererForFileSystemView(
        const QFileInfo &pathInfo, bool useDefaultFolderCover) = 0;

    virtual AbstractGalleryViewRenderer *createViewRenderer(
        QGraphicsScene *scene);
};

#endif // ABSTRACTRENDERERFACTORY_H
