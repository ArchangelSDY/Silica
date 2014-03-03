#ifndef LOOSERENDERERFACTORY_H
#define LOOSERENDERERFACTORY_H

#include "AbstractRendererFactory.h"

class LooseRendererFactory : public AbstractRendererFactory
{
public:
    AbstractGalleryItemRenderer *createForImageGallery();
    AbstractGalleryItemRenderer *createForPlayListGallery(const QString &);
};

#endif // LOOSERENDERERFACTORY_H
