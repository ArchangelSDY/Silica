#ifndef COMPACTRENDERERFACTORY_H
#define COMPACTRENDERERFACTORY_H

#include "AbstractRendererFactory.h"

class CompactRendererFactory : public AbstractRendererFactory
{
public:
    AbstractGalleryItemRenderer *createForImageGallery();
    AbstractGalleryItemRenderer *createForPlayListGallery(
        const QString &title, const int count);
};

#endif // COMPACTRENDERERFACTORY_H
