#include "AbstractRendererFactory.h"
#include "GridGalleryViewRenderer.h"

AbstractGalleryViewRenderer *AbstractRendererFactory::createViewRenderer(
        GalleryView *galleryView)
{
    return new GridGalleryViewRenderer(galleryView);
}
