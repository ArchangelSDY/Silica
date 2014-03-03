#include "LooseImageBackgroundRenderer.h"

LooseImageBackgroundRenderer::LooseImageBackgroundRenderer(
    AbstractGalleryItemRenderer *parentRenderer) :
    AbstractGalleryItemRenderer(parentRenderer)
{
}

void LooseImageBackgroundRenderer::layout()
{
    if (!m_parentRenderer) {
        return;
    }

    m_parentRenderer->layout();
}

void LooseImageBackgroundRenderer::paint(QPainter *painter)
{
    if (!m_parentRenderer) {
        return;
    }

    QColor c(255, 255, 255 ,172);
    painter->setBrush(c);
    painter->setPen(c);

    QRect rect = m_parentRenderer->boundingRect();
    rect.moveTo(-rect.width() / 2, -rect.height() / 2);

    painter->save();
    painter->translate(m_parentRenderer->boundingRect().center());

    painter->save();
    painter->rotate(LooseImageBackgroundRenderer::ANGLE);
    painter->drawRoundedRect(rect,
        LooseImageBackgroundRenderer::RADIUS, LooseImageBackgroundRenderer::RADIUS);
    painter->restore();

    painter->save();
    painter->rotate(-LooseImageBackgroundRenderer::ANGLE);
    painter->drawRoundedRect(rect,
        LooseImageBackgroundRenderer::RADIUS, LooseImageBackgroundRenderer::RADIUS);
    painter->restore();

    painter->restore();

    m_parentRenderer->paint(painter);
}
