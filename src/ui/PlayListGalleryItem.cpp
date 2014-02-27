#include <QGraphicsScene>
#include <QPainter>

#include "CompactImageRenderer.h"
#include "CompactTitleRenderer.h"
#include "GlobalConfig.h"
#include "PlayListGalleryItem.h"

PlayListGalleryItem::PlayListGalleryItem(PlayListRecord *record,
                                         QGraphicsItem *parent) :
    QGraphicsItem(parent) ,
    m_record(record) ,
    m_image(0) ,
    m_renderer(new CompactTitleRenderer(record->name(), new CompactImageRenderer()))
{
    setFlag(QGraphicsItem::ItemIsSelectable);

    loadThumbnail();
}

PlayListGalleryItem::~PlayListGalleryItem()
{
    delete m_record;
    if (m_image) {
        delete m_image;
    }
}

void PlayListGalleryItem::loadThumbnail()
{
    if (m_image) {
        delete m_image;
    }

    m_image = new QImage(m_record->coverPath());
    m_renderer->setImage(const_cast<QImage *>(m_image));
    m_renderer->layout();

    update(boundingRect());
}

QRectF PlayListGalleryItem::boundingRect() const
{
    return QRectF(QPointF(0, 0), GlobalConfig::instance()->galleryItemSize());
}

void PlayListGalleryItem::paint(QPainter *painter,
                                const QStyleOptionGraphicsItem *,
                                QWidget *)
{
    painter->setRenderHints(
        QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    painter->setPen(Qt::NoPen);
    painter->setBackground(scene()->palette().background());
    painter->setBrush(scene()->palette().foreground());

    // Background
    if (isSelected()) {
        QRadialGradient gradient(boundingRect().center(),
                                 boundingRect().width());
        gradient.setColorAt(0, painter->brush().color());
        gradient.setColorAt(1, Qt::transparent);
        painter->setBrush(gradient);

        painter->fillRect(boundingRect(), QBrush(gradient));
    } else {
        painter->eraseRect(boundingRect());
    }

    m_renderer->paint(painter);
}
