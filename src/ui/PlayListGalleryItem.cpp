#include <QGraphicsScene>
#include <QPainter>

#include "CompactImageRenderer.h"
#include "GlobalConfig.h"
#include "PlayListGalleryItem.h"

static const int TITLE_HEIGHT = 25;
static const int BORDER = 1;

PlayListGalleryItem::PlayListGalleryItem(PlayListRecord *record,
                                         QGraphicsItem *parent) :
    QGraphicsItem(parent) ,
    m_record(record) ,
    m_image(0) ,
    m_renderer(new CompactImageRenderer())
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

//    m_titleRect.setRect(m_innerRect.x(),
//                        m_innerRect.y() + m_innerRect.height() - TITLE_HEIGHT,
//                        m_innerRect.width(), TITLE_HEIGHT);

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

    // Title
//    painter->setBrush(QColor(255, 255, 255, 200));
//    painter->drawRect(m_titleRect);
//    painter->setPen(Qt::black);

//    painter->drawText(m_titleRect, Qt::AlignCenter | Qt::AlignHCenter,
//                      m_record->name());
}
