#include <QGraphicsScene>

#include "GalleryView.h"
#include "GlobalConfig.h"
#include "ImageGalleryItem.h"
#include "LooseImageRenderer.h"

ImageGalleryItem::ImageGalleryItem(ImagePtr image,
                                   AbstractRendererFactory *rendererFactory,
                                   QGraphicsItem *parent) :
    GalleryItem(rendererFactory, parent) ,
    m_image(image)
{
    setFlag(QGraphicsItem::ItemIsSelectable);
    createRenderer();

    connect(m_image.data(), &Image::thumbnailLoaded,
            this, &ImageGalleryItem::thumbnailLoaded);
    connect(m_image.data(), &Image::thumbnailLoadFailed,
            this, &ImageGalleryItem::thumbnailLoadFailed);
}

void ImageGalleryItem::load()
{
    m_image->loadThumbnail();
}

void ImageGalleryItem::createRenderer()
{
    setRenderer(m_rendererFactory->createItemRendererForImageGallery());
}

QString ImageGalleryItem::name() const
{
    return !m_image.isNull() ? m_image->name() : QString();
}

QRectF ImageGalleryItem::boundingRect() const
{
    return m_renderer->boundingRect();
}

void ImageGalleryItem::thumbnailLoaded(QSharedPointer<QImage> thumbnail)
{
    QImage *image = new QImage(*thumbnail);
    setThumbnail(image);
}

void ImageGalleryItem::thumbnailLoadFailed()
{
    setThumbnail(new QImage());
}
