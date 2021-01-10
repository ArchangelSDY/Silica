#ifndef LOOSERENDERERFACTORY_H
#define LOOSERENDERERFACTORY_H

#include "AbstractRendererFactory.h"

class LooseRendererFactory : public AbstractRendererFactory
{
public:
    AbstractGalleryItemRenderer *createItemRendererForImageGallery();
    AbstractGalleryItemRenderer *createItemRendererForPlayListGallery(
        const QString &, std::optional<int>);
    AbstractGalleryItemRenderer *createItemRendererForFileSystemView(
        const QFileInfo &, bool);
};

#endif // LOOSERENDERERFACTORY_H
