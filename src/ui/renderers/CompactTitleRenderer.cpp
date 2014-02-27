#include "CompactTitleRenderer.h"

const int CompactTitleRenderer::TITLE_HEIGHT = 25;

CompactTitleRenderer::CompactTitleRenderer(
    const QString &title, AbstractGalleryItemRenderer *parentRenderer) :
    AbstractGalleryItemRenderer(parentRenderer) ,
    m_title(title)
{
}

void CompactTitleRenderer::layout()
{
    if (!m_parentRenderer) {
        return;
    }

    m_parentRenderer->layout();

    const QRect &parentBoundingRect = m_parentRenderer->boundingRect();
    int top = parentBoundingRect.y() + parentBoundingRect.height() -
        CompactTitleRenderer::TITLE_HEIGHT;
    m_titleRect.setRect(parentBoundingRect.x(), top,
                        parentBoundingRect.width(),
                        CompactTitleRenderer::TITLE_HEIGHT);
}

void CompactTitleRenderer::paint(QPainter *painter)
{
    if (m_parentRenderer) {
        m_parentRenderer->paint(painter);
    }

    painter->setBrush(QColor(255, 255, 255, 200));
    painter->drawRect(m_titleRect);
    painter->setPen(Qt::black);

    painter->drawText(m_titleRect, Qt::AlignCenter | Qt::AlignHCenter,
                      m_title);
}
