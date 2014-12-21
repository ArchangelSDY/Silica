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
    setRenderer(m_rendererFactory->createItemRendererForImageGallery());

    connect(m_image.data(), SIGNAL(thumbnailLoaded()),
            this, SLOT(thumbnailLoaded()));
    m_image->loadThumbnail(true);
}

void ImageGalleryItem::setRendererFactory(AbstractRendererFactory *factory)
{
    m_rendererFactory = factory;
    setRenderer(m_rendererFactory->createItemRendererForImageGallery());
}

QRectF ImageGalleryItem::boundingRect() const
{
    return m_renderer->boundingRect();
}

void ImageGalleryItem::thumbnailLoaded()
{
    const QImage &thumbnailImage = m_image->thumbnail();
    if (!thumbnailImage.isNull()) {
        m_thumbnail = new QImage(thumbnailImage);
        m_renderer->setImage(const_cast<QImage *>(m_thumbnail));

        prepareGeometryChange();
        m_renderer->layout();

        update(boundingRect());

        if (scene()) {
            QList<QGraphicsView *> views = scene()->views();
            if (!views.isEmpty()) {
                GalleryView *view = static_cast<GalleryView *>(views[0]);
                view->scheduleLayout();
            }
        }

        emit readyToShow();
    }
}
