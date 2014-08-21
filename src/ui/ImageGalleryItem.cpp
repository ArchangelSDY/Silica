#include <QGraphicsScene>
#include <QPainter>

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
    setRenderer(m_rendererFactory->createForImageGallery());

    connect(m_image.data(), SIGNAL(thumbnailLoaded()),
            this, SLOT(thumbnailLoaded()));
    m_image->loadThumbnail(true);
}

void ImageGalleryItem::setRendererFactory(AbstractRendererFactory *factory)
{
    m_rendererFactory = factory;
    setRenderer(m_rendererFactory->createForImageGallery());
}

QRectF ImageGalleryItem::boundingRect() const
{
    return QRectF(QPointF(0, 0), GlobalConfig::instance()->galleryItemSize());
}

void ImageGalleryItem::thumbnailLoaded()
{
    const QImage &thumbnailImage = m_image->thumbnail();
    if (!thumbnailImage.isNull()) {
        m_thumbnail = new QImage(thumbnailImage);
        m_renderer->setImage(const_cast<QImage *>(m_thumbnail));
        m_renderer->layout();
        update(boundingRect());
    }
}

void ImageGalleryItem::paint(QPainter *painter,
                        const QStyleOptionGraphicsItem *,
                        QWidget *)
{
    painter->setRenderHints(
        QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    painter->setBrush(scene()->palette().foreground());
    painter->setBackground(scene()->palette().background());

    // Background
    if (isSelected()) {
        painter->drawRect(boundingRect());
    } else {
        painter->eraseRect(boundingRect());
    }

    m_renderer->paint(painter);
}
