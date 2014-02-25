#include <QGraphicsScene>
#include <QPainter>

#include "GalleryItem.h"
#include "../GlobalConfig.h"

static const int PADDING = 10;
static const int BORDER = 5;

GalleryItem::GalleryItem(AbstractGalleryItemModel *model,
                         QGraphicsItem *parent) :
    QGraphicsItem(parent) ,
    m_model(model)
{
    setFlag(QGraphicsItem::ItemIsSelectable);

    if (m_model) {
        m_model->setParent(this);

        connect(m_model, SIGNAL(thumbnailLoaded()),
                this, SLOT(thumbnailLoaded()));
        m_model->loadThumbnail();
    }
}

GalleryItem::~GalleryItem()
{
    if (m_model) {
        delete m_model;
    }
}

QRectF GalleryItem::boundingRect() const
{
    return QRectF(QPointF(0, 0), GlobalConfig::instance()->galleryItemSize());
}

void GalleryItem::thumbnailLoaded()
{
    if (!m_model) {
        return;
    }

    const QImage &thumbnailImage = m_model->thumbnail();
    if (!thumbnailImage.isNull()) {
        // Calculate image size and position
        const QSize &itemSize = GlobalConfig::instance()->galleryItemSize();
        m_thumbnailSize.setWidth(itemSize.width() - 2 * PADDING);
        m_thumbnailSize.setHeight(itemSize.height() - 2 * PADDING);

        m_thumbnailSize = thumbnailImage.size().scaled(
            m_thumbnailSize, Qt::KeepAspectRatio);

        m_thumbnailPos.setX((itemSize.width() - m_thumbnailSize.width()) / 2);
        m_thumbnailPos.setY((itemSize.height() - m_thumbnailSize.height()) / 2);

        m_borderRect.setX(m_thumbnailPos.x() - BORDER);
        m_borderRect.setY(m_thumbnailPos.y() - BORDER);
        m_borderRect.setWidth(m_thumbnailSize.width() + 2 * BORDER);
        m_borderRect.setHeight(m_thumbnailSize.height() + 2 * BORDER);

        update(boundingRect());
    }
}

void GalleryItem::paint(QPainter *painter,
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

    // Border
    painter->setBrush(Qt::white);
    painter->setPen(Qt::white);
    painter->drawRoundedRect(m_borderRect, BORDER, BORDER);

    // Image
    painter->drawImage(QRect(m_thumbnailPos, m_thumbnailSize),
                       m_model->thumbnail());
}
