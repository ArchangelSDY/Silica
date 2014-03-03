#ifndef ABSTRACTRENDERERFACTORY_H
#define ABSTRACTRENDERERFACTORY_H

#include "AbstractGalleryItemRenderer.h"

class AbstractRendererFactory
{
public:
    virtual AbstractGalleryItemRenderer *createForImageGallery() = 0;
    virtual ~AbstractRendererFactory() {}
};

#endif // ABSTRACTRENDERERFACTORY_H
