#include <QVariant>

#include "GalleryItem.h"

GalleryItem::GalleryItem(AbstractRendererFactory *rendererFactory,
                         QGraphicsItem *parent) :
    QGraphicsItem(parent) ,
    m_rendererFactory(rendererFactory) ,
    m_renderer(0) ,
    m_thumbnail(0) ,
    m_selectedAfterShownScheduled(false)
{
    // Hide until thumbnail is ready.
    // It will show during next view layout after thumbnail is loaded
    //
    // This strategy is to hide the crazy rapid layout procedure when loading
    // remote gallery in waterfall mode.
    hide();
}

GalleryItem::~GalleryItem()
{
    if (m_renderer) {
        delete m_renderer;
    }
    if (m_thumbnail) {
        delete m_thumbnail;
    }
}

void GalleryItem::setRenderer(AbstractGalleryItemRenderer *renderer)
{
    delete m_renderer;
    m_renderer = renderer;
    m_renderer->setImage(m_thumbnail);

    prepareGeometryChange();
    m_renderer->layout();
}

bool GalleryItem::isReadyToShow()
{
    return m_thumbnail && !m_thumbnail->isNull();
}

void GalleryItem::scheduleSelectedAfterShown()
{
    m_selectedAfterShownScheduled = true;
}

QVariant GalleryItem::itemChange(GraphicsItemChange change,
                                 const QVariant &value)
{
    if (change == ItemVisibleHasChanged) {
        if (value.toBool() && m_selectedAfterShownScheduled) {
            setSelected(true);
            m_selectedAfterShownScheduled = false;
        }
    }
    return QGraphicsItem::itemChange(change, value);
}
