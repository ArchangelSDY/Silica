#include <QPainter>

#include "GlobalConfig.h"
#include "PlayListGalleryItem.h"

static const int TITLE_HEIGHT = 25;
static const int BORDER = 5;

PlayListGalleryItem::PlayListGalleryItem(PlayListRecord *record,
                                         QGraphicsItem *parent) :
    QGraphicsItem(parent) ,
    m_record(record) ,
    m_image(0)
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

    // Calculate image size and position
    QSize margins(2 * BORDER, 2 * BORDER);
    m_innerRect.setSize(GlobalConfig::instance()->galleryItemSize() - margins);
    m_innerRect.translate(BORDER, BORDER);
    const QSize &coverSize = m_image->size();

    QSize sourcePaintSize = m_innerRect.size().scaled(
        coverSize, Qt::KeepAspectRatio);
    m_coverSourcePaintRect.setSize(sourcePaintSize);
    m_coverSourcePaintRect.translate(
        (coverSize.width() - sourcePaintSize.width()) / 2,
        (coverSize.height() - sourcePaintSize.height()) / 2
    );

    m_titleRect.setRect(m_innerRect.x(),
                        m_innerRect.y() + m_innerRect.height() - TITLE_HEIGHT,
                        m_innerRect.width(), TITLE_HEIGHT);

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

    // Background
    if (isSelected()) {
        QRadialGradient gradient(boundingRect().center(),
                                 boundingRect().width());
        gradient.setColorAt(0, Qt::darkCyan);
        gradient.setColorAt(1, Qt::transparent);
        painter->setBrush(gradient);
        painter->setPen(Qt::NoPen);

        painter->fillRect(boundingRect(), QBrush(gradient));
    } else {
        painter->fillRect(boundingRect(), Qt::gray);
    }

    // Image
    painter->drawImage(m_innerRect, *m_image, m_coverSourcePaintRect);

    // Title
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(255, 255, 255, 200));
    painter->drawRect(m_titleRect);
    painter->setPen(Qt::black);

    painter->drawText(m_titleRect, Qt::AlignCenter | Qt::AlignHCenter,
                      m_record->name());
}
