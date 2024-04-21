#pragma once
#include <QPainter>

class AbstractGalleryItemRenderer
{
public:
    AbstractGalleryItemRenderer(AbstractGalleryItemRenderer *parentRenderer = 0);
    virtual ~AbstractGalleryItemRenderer();

    void setImage(QSharedPointer<QImage> image);
    void setImageSize(const QSize &size);

    virtual Qt::AspectRatioMode aspectRatioMode() const;
    virtual QRect boundingRect() const;
    virtual void layout(const QRect &viewGeometry);
    virtual void paint(QPainter *painter);

protected:
    AbstractGalleryItemRenderer *m_parentRenderer;
    QRect m_boundingRect;
    QSharedPointer<QImage> m_image;
    QSize m_imageSize;
};
