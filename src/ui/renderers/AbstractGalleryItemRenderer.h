#ifndef ABSTRACTGALLERYITEMRENDERER_H
#define ABSTRACTGALLERYITEMRENDERER_H

#include <QPainter>

class AbstractGalleryItemRenderer
{
public:
    AbstractGalleryItemRenderer(AbstractGalleryItemRenderer *parentRenderer = 0);
    virtual ~AbstractGalleryItemRenderer();

    void setImage(const QImage *image, bool isOwnImage = false);
    void setImageSize(const QSize &size);

    virtual Qt::AspectRatioMode aspectRatioMode() const;
    virtual QRect boundingRect() const;
    virtual void layout();
    virtual void paint(QPainter *painter);

protected:
    AbstractGalleryItemRenderer *m_parentRenderer;
    QRect m_boundingRect;
    const QImage *m_image;
    QSize m_imageSize;
    bool m_isOwnImage;
};

#endif // ABSTRACTGALLERYITEMRENDERER_H
