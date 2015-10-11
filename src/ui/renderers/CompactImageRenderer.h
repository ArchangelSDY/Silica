#ifndef COMPACTIMAGERENDERER_H
#define COMPACTIMAGERENDERER_H

#include "AbstractGalleryItemRenderer.h"
#include "GlobalConfig.h"

class CompactImageRenderer : public AbstractGalleryItemRenderer
{
public:
    CompactImageRenderer(
        AbstractGalleryItemRenderer *parentRenderer = 0,
        QImage *image = 0,
        const QRect &boundingRect = QRect(
            QPoint(0, 0), GlobalConfig::instance()->galleryItemSize()));

    virtual Qt::AspectRatioMode aspectRatioMode() const;
    virtual void layout();
    virtual void paint(QPainter *painter);

    AbstractGalleryItemRenderer *setBorder(
        int border, const QColor &color = Qt::transparent);
    AbstractGalleryItemRenderer *setPadding(
        int padding, const QColor &color = Qt::transparent);
    AbstractGalleryItemRenderer *translate(int dx, int dy);

private:
    QRect m_imageRect;
    QRect m_imageSourceRect;
    int m_border;
    QColor m_borderColor;
    int m_padding;
    QColor m_paddingColor;
    int m_translateX;
    int m_translateY;
};

#endif // COMPACTIMAGERENDERER_H
