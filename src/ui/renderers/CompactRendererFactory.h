#ifndef COMPACTRENDERERFACTORY_H
#define COMPACTRENDERERFACTORY_H

#include "AbstractRendererFactory.h"

class CompactRendererFactory : public AbstractRendererFactory
{
public:
    AbstractGalleryItemRenderer *createForImageGallery();
    AbstractGalleryItemRenderer *createForPlayListGallery(const QString &title);
};

#endif // COMPACTRENDERERFACTORY_H
