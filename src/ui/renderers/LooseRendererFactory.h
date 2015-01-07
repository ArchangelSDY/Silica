#ifndef LOOSERENDERERFACTORY_H
#define LOOSERENDERERFACTORY_H

#include "AbstractRendererFactory.h"

class LooseRendererFactory : public AbstractRendererFactory
{
public:
    AbstractGalleryItemRenderer *createItemRendererForImageGallery();
    AbstractGalleryItemRenderer *createItemRendererForPlayListGallery(
        const QString &, const int);
    AbstractGalleryItemRenderer *createItemRendererForFileSystemView(
        const QString &title);
};

#endif // LOOSERENDERERFACTORY_H
