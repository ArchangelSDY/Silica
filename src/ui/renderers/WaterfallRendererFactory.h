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
        const QFileInfo &, bool);

    virtual AbstractGalleryViewRenderer *createViewRenderer(
        GalleryView *galleryView);
};

#endif // WATERFALLRENDERERFACTORY_H
