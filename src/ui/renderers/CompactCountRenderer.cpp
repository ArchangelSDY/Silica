#include "CompactCountRenderer.h"

const int CompactCountRenderer::CORNER_WIDTH = 50;
const char *CompactCountRenderer::CORNER_BRUSH = "#AA0099EE";

CompactCountRenderer::CompactCountRenderer(
    const int count, AbstractGalleryItemRenderer *parentRenderer) :
    AbstractGalleryItemRenderer(parentRenderer) ,
    m_count(count)
{
}

void CompactCountRenderer::paint(QPainter *painter)
{
    if (m_parentRenderer) {
        m_parentRenderer->paint(painter);
    }

    if (m_count < 0) {
        return;
    }

    const QRect &parentBounding = m_parentRenderer->boundingRect();
    const QPoint &topRight = parentBounding.topRight();

    QPolygon corner;
    corner << QPoint(topRight.x() - CompactCountRenderer::CORNER_WIDTH,
                     topRight.y())
           << QPoint(topRight.x() + 1,
                     topRight.y() + CompactCountRenderer::CORNER_WIDTH)
           << QPoint(topRight.x() + 1, topRight.y());
    painter->save();
    painter->setPen(Qt::NoPen);
    painter->setBrush(QBrush(CompactCountRenderer::CORNER_BRUSH));
    painter->drawPolygon(corner);
    painter->restore();

    const QPoint &topLeft = parentBounding.topLeft();
    painter->save();
    painter->setPen(Qt::white);
    painter->translate(parentBounding.topRight());
    painter->rotate(45);
    int fontSize = painter->font().pointSize();
    QRect textRect(topLeft.x() - fontSize * 3,
                   topLeft.y() - fontSize,
                   fontSize * 6, fontSize * 2);
    textRect.translate(0, CompactCountRenderer::CORNER_WIDTH * 0.4);
    painter->drawText(textRect, Qt::AlignHCenter | Qt::AlignVCenter,
                      QString::number(m_count));
    painter->restore();
}
