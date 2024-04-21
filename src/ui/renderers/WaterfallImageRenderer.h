#pragma once

#include "AbstractGalleryItemRenderer.h"

class WaterfallImageRenderer : public AbstractGalleryItemRenderer
{
public:
    WaterfallImageRenderer(int maxColumns);

    virtual Qt::AspectRatioMode aspectRatioMode() const;
    virtual void layout(const QRect &viewGeometry);
    virtual void paint(QPainter *painter);

    int columnsSpan() const;

private:
    static const int PADDING;
    static const int BORDER;

    int m_maxColumns;
    QRect m_imageRect;
    QRect m_borderRect;
};
