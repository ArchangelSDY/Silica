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
        const QString &title);
};

#endif // COMPACTRENDERERFACTORY_H
