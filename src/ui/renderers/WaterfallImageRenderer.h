#pragma once

#include "AbstractGalleryItemRenderer.h"

class WaterfallImageRenderer : public AbstractGalleryItemRenderer
{
public:
    WaterfallImageRenderer(int itemWidth);

    virtual Qt::AspectRatioMode aspectRatioMode() const;
    virtual void layout();
    virtual void paint(QPainter *painter);

    int columnsSpan() const;

private:
    static const int PADDING;
    static const int BORDER;

    int m_itemWidth;
    QRect m_imageRect;
    QRect m_borderRect;
};
