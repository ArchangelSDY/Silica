#include "CompactCornerIconRenderer.h"

const int CompactCornerIconRenderer::CORNER_WIDTH = 50;
const char *CompactCornerIconRenderer::CORNER_BRUSH = "#AA0099EE";

CompactCornerIconRenderer::CompactCornerIconRenderer(
        const QImage &icon, AbstractGalleryItemRenderer *parentRenderer) :
    AbstractGalleryItemRenderer(parentRenderer) ,
    m_icon(icon)
{
}

void CompactCornerIconRenderer::paint(QPainter *painter)
{
    AbstractGalleryItemRenderer::paint(painter);

    const QPoint &topRight = boundingRect().topRight();

    // Background
    QPolygon corner;
    corner << QPoint(topRight.x() - CompactCornerIconRenderer::CORNER_WIDTH,
                     topRight.y())
           << QPoint(topRight.x() + 1,
                     topRight.y() + CompactCornerIconRenderer::CORNER_WIDTH)
           << QPoint(topRight.x() + 1, topRight.y());
    painter->save();
    painter->setPen(Qt::NoPen);
    painter->setBrush(QBrush(CompactCornerIconRenderer::CORNER_BRUSH));
    painter->drawPolygon(corner);
    painter->restore();

    // Icon
    const QRect &cornerRect = corner.boundingRect();
    QRect iconRect = cornerRect;
    iconRect.setBottomLeft(cornerRect.center());
    painter->drawImage(iconRect, m_icon);
}

