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
        const QRect &imageRect = QRect(
            QPoint(BORDER, BORDER),
            GlobalConfig::instance()->galleryItemSize()
                - QSize(2 * BORDER, 2 * BORDER)));

    virtual void layout();
    virtual void paint(QPainter *painter);

private:
    static const int BORDER;

    QRect m_imageRect;
    QRect m_imageSourceRect;
};

#endif // COMPACTIMAGERENDERER_H
