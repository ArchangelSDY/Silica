#ifndef IMAGEGALLERYITEM_H
#define IMAGEGALLERYITEM_H

#include <QGraphicsItem>

#include "ui/renderers/AbstractGalleryItemRenderer.h"
#include "ui/GalleryItem.h"
#include "image/Image.h"

class ImageGalleryItem : public GalleryItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    explicit ImageGalleryItem(ImagePtr image,
                              AbstractRendererFactory *rendererFactory,
                              QGraphicsItem *parent = 0);

    QString name() const;
    QRectF boundingRect() const;
    ImagePtr image() { return m_image; }

    virtual void load() override;
    virtual void unload() override;
    virtual void createRenderer();

private slots:
    void thumbnailLoaded(QSharedPointer<QImage> thumbnail);
    void thumbnailLoadFailed();

private:
    ImagePtr m_image;
};

#endif // IMAGEGALLERYITEM_H
