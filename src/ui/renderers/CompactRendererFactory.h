#ifndef COMPACTRENDERERFACTORY_H
#define COMPACTRENDERERFACTORY_H

#include "AbstractRendererFactory.h"

class CompactRendererFactory : public AbstractRendererFactory
{
public:
    AbstractGalleryItemRenderer *createRenderer();
};

#endif // COMPACTRENDERERFACTORY_H
