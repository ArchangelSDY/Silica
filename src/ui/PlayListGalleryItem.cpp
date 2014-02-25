#include <QPainter>

#include "GlobalConfig.h"
#include "PlayListGalleryItem.h"

static const int TITLE_HEIGHT = 25;

PlayListGalleryItem::PlayListGalleryItem(PlayListRecord *record,
                                         QGraphicsItem *parent) :
    AbstractGalleryItem(parent) ,
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
    const QSize &itemSize = GlobalConfig::instance()->galleryItemSize();
    m_thumbnailSize.setWidth(itemSize.width());
    m_thumbnailSize.setHeight(itemSize.height());

    m_thumbnailSize = m_image->size().scaled(
        m_thumbnailSize, Qt::KeepAspectRatio);

    m_thumbnailPos.setX((itemSize.width() - m_thumbnailSize.width()) / 2);
    m_thumbnailPos.setY((itemSize.height() - m_thumbnailSize.height()) / 2);

    int titleY = m_thumbnailPos.y() + m_thumbnailSize.height() - TITLE_HEIGHT;
    m_titleRect.setRect(m_thumbnailPos.x(), titleY,
                        m_thumbnailSize.width(), TITLE_HEIGHT);

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
        painter->fillRect(boundingRect(), Qt::darkCyan);
    } else {
        painter->fillRect(boundingRect(), Qt::gray);
    }

    // Image
    painter->drawImage(QRect(m_thumbnailPos, m_thumbnailSize),
                       *m_image);

    // Title
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(255, 255, 255, 200));
    painter->drawRect(m_titleRect);
    painter->setPen(Qt::black);

    painter->drawText(m_titleRect, Qt::AlignCenter | Qt::AlignHCenter,
                      m_record->name());
}
