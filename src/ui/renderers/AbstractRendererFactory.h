#ifndef ABSTRACTRENDERERFACTORY_H
#define ABSTRACTRENDERERFACTORY_H

#include "AbstractGalleryItemRenderer.h"

class AbstractRendererFactory
{
public:
    virtual AbstractGalleryItemRenderer *createForImageGallery() = 0;
    virtual AbstractGalleryItemRenderer *createForPlayListGallery(
        const QString &title, const int count) = 0;
    virtual ~AbstractRendererFactory() {}
};

#endif // ABSTRACTRENDERERFACTORY_H
