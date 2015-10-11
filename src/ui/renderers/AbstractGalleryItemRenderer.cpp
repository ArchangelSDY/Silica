#include "AbstractGalleryItemRenderer.h"

AbstractGalleryItemRenderer::AbstractGalleryItemRenderer(
    AbstractGalleryItemRenderer *parentRenderer) :
    m_parentRenderer(parentRenderer) ,
    m_image(0) ,
    m_isOwnImage(false)
{
}

AbstractGalleryItemRenderer::~AbstractGalleryItemRenderer()
{
    if (m_parentRenderer) {
        delete m_parentRenderer;
    }
    if (m_isOwnImage && m_image) {
        delete m_image;
    }
}

void AbstractGalleryItemRenderer::setImage(const QImage *image,
                                           bool isOwnImage)
{
    // Should always set new image here to be consistent with
    // GalleryItem::setThumbnail(), which always deletes the old one.
    if (m_image && m_isOwnImage) {
        delete m_image;
    }
    m_image = image;
    m_isOwnImage = isOwnImage;
    if (m_parentRenderer) {
        m_parentRenderer->setImage(image);
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
