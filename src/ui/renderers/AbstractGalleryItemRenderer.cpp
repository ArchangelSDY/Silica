#include "AbstractGalleryItemRenderer.h"

AbstractGalleryItemRenderer::AbstractGalleryItemRenderer(
    AbstractGalleryItemRenderer *parentRenderer) :
    m_parentRenderer(parentRenderer) ,
    m_image(0)
{
}

AbstractGalleryItemRenderer::~AbstractGalleryItemRenderer()
{
    if (m_parentRenderer) {
        delete m_parentRenderer;
    }
}

void AbstractGalleryItemRenderer::setImage(const QImage *image)
{
    m_image = image;
    if (m_parentRenderer) {
        m_parentRenderer->setImage(image);
    }
}

void AbstractGalleryItemRenderer::layout()
{
    if (m_parentRenderer) {
        m_parentRenderer->layout();
    }
}

void AbstractGalleryItemRenderer::paint(QPainter *painter)
{
    if (m_parentRenderer) {
        m_parentRenderer->paint(painter);
    }
}
