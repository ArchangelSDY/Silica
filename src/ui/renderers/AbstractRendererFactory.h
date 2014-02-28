#ifndef ABSTRACTRENDERERFACTORY_H
#define ABSTRACTRENDERERFACTORY_H

#include "AbstractGalleryItemRenderer.h"

class AbstractRendererFactory
{
public:
    virtual AbstractGalleryItemRenderer *createRenderer() = 0;
};

#endif // ABSTRACTRENDERERFACTORY_H
