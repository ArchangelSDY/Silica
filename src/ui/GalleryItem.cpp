#include <QPalette>
#include <QVariant>

#include "GalleryItem.h"

const QColor GalleryItem::SELECTED_COLOR = QColor("#AAFFFFFF");

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

void GalleryItem::paint(QPainter *painter,
                        const QStyleOptionGraphicsItem *option,
                        QWidget *widget)
{
    painter->setRenderHints(
        QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    painter->setPen(Qt::NoPen);
    painter->setBackground(scene()->palette().background());
    painter->setBrush(scene()->palette().foreground());

    // Clear painter first
    painter->eraseRect(boundingRect());

    // Render
    m_renderer->paint(painter);

    // Draw a mask if selected
    if (isSelected()) {
        painter->fillRect(boundingRect(), GalleryItem::SELECTED_COLOR);
    }
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
