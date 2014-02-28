#ifndef LOOSERENDERERFACTORY_H
#define LOOSERENDERERFACTORY_H

#include "AbstractRendererFactory.h"

class LooseRendererFactory : public AbstractRendererFactory
{
public:
    AbstractGalleryItemRenderer *createRenderer();
};

#endif // LOOSERENDERERFACTORY_H
