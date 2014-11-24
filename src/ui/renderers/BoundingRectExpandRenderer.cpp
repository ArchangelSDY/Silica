#include "BoundingRectExpandRenderer.h"

BoundingRectExpandRenderer::BoundingRectExpandRenderer(
        const QRect &expandedRect,
        AbstractGalleryItemRenderer *parentRenderer) :
    AbstractGalleryItemRenderer(parentRenderer)
{
    m_boundingRect = expandedRect;
}
