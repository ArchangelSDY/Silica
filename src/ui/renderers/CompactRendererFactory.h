#ifndef COMPACTRENDERERFACTORY_H
#define COMPACTRENDERERFACTORY_H

#include "AbstractRendererFactory.h"

class CompactRendererFactory : public AbstractRendererFactory
{
public:
    AbstractGalleryItemRenderer *createItemRendererForImageGallery();
    AbstractGalleryItemRenderer *createItemRendererForPlayListGallery(
        const QString &title, const int count);
    AbstractGalleryItemRenderer *createItemRendererForFileSystemView(
        const QFileInfo &pathInfo);
};

#endif // COMPACTRENDERERFACTORY_H
