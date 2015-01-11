#ifndef WATERFALLRENDERERFACTORY_H
#define WATERFALLRENDERERFACTORY_H

#include "AbstractRendererFactory.h"

class WaterfallRendererFactory : public AbstractRendererFactory
{
public:
    virtual AbstractGalleryItemRenderer *createItemRendererForImageGallery();
    virtual AbstractGalleryItemRenderer *createItemRendererForPlayListGallery(
        const QString &title, const int count);
    virtual AbstractGalleryItemRenderer *createItemRendererForFileSystemView(
        const QFileInfo &);

    virtual AbstractGalleryViewRenderer *createViewRenderer(
        QGraphicsScene *scene);
};

#endif // WATERFALLRENDERERFACTORY_H
