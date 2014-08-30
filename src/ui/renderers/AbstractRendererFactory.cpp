#include "AbstractRendererFactory.h"
#include "GridGalleryViewRenderer.h"

AbstractGalleryViewRenderer *AbstractRendererFactory::createViewRenderer(
        QGraphicsScene *scene)
{
    return new GridGalleryViewRenderer(scene);
}
