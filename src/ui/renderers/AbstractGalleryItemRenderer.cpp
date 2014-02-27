#include "AbstractGalleryItemRenderer.h"

AbstractGalleryItemRenderer::AbstractGalleryItemRenderer(
    AbstractGalleryItemRenderer *parentRenderer) :
    m_parentRenderer(parentRenderer) ,
    m_image(0)
{
}

void AbstractGalleryItemRenderer::setImage(const QImage *image)
{
    m_image = image;
    if (m_parentRenderer) {
        m_parentRenderer->setImage(image);
    }
}
