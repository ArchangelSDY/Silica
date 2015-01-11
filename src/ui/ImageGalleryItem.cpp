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

    connect(m_image.data(), SIGNAL(thumbnailLoaded()),
            this, SLOT(thumbnailLoaded()));
    connect(m_image.data(), SIGNAL(thumbnailLoadFailed()),
            this, SLOT(thumbnailLoaded()));
}

void ImageGalleryItem::load()
{
    m_image->loadThumbnail(true);
}

void ImageGalleryItem::createRenderer()
{
    setRenderer(m_rendererFactory->createItemRendererForImageGallery());
}

QRectF ImageGalleryItem::boundingRect() const
{
    return m_renderer->boundingRect();
}

void ImageGalleryItem::thumbnailLoaded()
{
    const QImage &thumbnailImage = m_image->thumbnail();
    setThumbnail(new QImage(thumbnailImage));
}
