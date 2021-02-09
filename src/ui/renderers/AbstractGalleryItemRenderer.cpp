#include "AbstractGalleryItemRenderer.h"

AbstractGalleryItemRenderer::AbstractGalleryItemRenderer(
    AbstractGalleryItemRenderer *parentRenderer) :
    m_parentRenderer(parentRenderer)
{
}

AbstractGalleryItemRenderer::~AbstractGalleryItemRenderer()
{
    if (m_parentRenderer) {
        delete m_parentRenderer;
    }
}

void AbstractGalleryItemRenderer::setImage(QSharedPointer<QImage> image)
{
    m_image = image;
    if (m_image) {
        m_imageSize = m_image->size();
    }
    if (m_parentRenderer) {
        m_parentRenderer->setImage(image);
    }
}

void AbstractGalleryItemRenderer::setImageSize(const QSize &size)
{
    m_imageSize = size;
    if (m_parentRenderer) {
        m_parentRenderer->setImageSize(size);
    }
}

Qt::AspectRatioMode AbstractGalleryItemRenderer::aspectRatioMode() const
{
    return m_parentRenderer ? m_parentRenderer->aspectRatioMode() : Qt::KeepAspectRatio;
}

QRect AbstractGalleryItemRenderer::boundingRect() const
{
    if (m_parentRenderer) {
        return m_boundingRect.united(m_parentRenderer->boundingRect());
    } else {
        return m_boundingRect;
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
