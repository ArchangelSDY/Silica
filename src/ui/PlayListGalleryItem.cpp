#include <QGraphicsScene>
#include <QPainter>

#include "CompactImageRenderer.h"
#include "CompactTitleRenderer.h"
#include "GlobalConfig.h"
#include "LooseImageRenderer.h"
#include "LooseImageBackgroundRenderer.h"
#include "PlayListGalleryItem.h"

PlayListGalleryItem::PlayListGalleryItem(PlayListRecord *record,
                                         AbstractRendererFactory *rendererFactory,
                                         QGraphicsItem *parent) :
    GalleryItem(rendererFactory, parent) ,
    m_record(record)
{
    setFlag(QGraphicsItem::ItemIsSelectable);
    setToolTip(m_record->name());
    setRenderer(m_rendererFactory->createForPlayListGallery(m_record->name()));

    connect(m_record, SIGNAL(saved()), this, SLOT(loadThumbnail()));

    loadThumbnail();
}

PlayListGalleryItem::~PlayListGalleryItem()
{
    delete m_record;
}

void PlayListGalleryItem::loadThumbnail()
{
    if (!m_rendererFactory) {
        return;
    }

    if (m_thumbnail) {
        delete m_thumbnail;
    }
    m_thumbnail = new QImage(m_record->coverPath());
    if (m_thumbnail->isNull()) {
        m_thumbnail->load(":/res/album.png");
    }

    // Replace with new render since cover image has changed
    setRenderer(m_rendererFactory->createForPlayListGallery(m_record->name()));
}

void PlayListGalleryItem::setRendererFactory(AbstractRendererFactory *factory)
{
    m_rendererFactory = factory;
    setRenderer(m_rendererFactory->createForPlayListGallery(m_record->name()));
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

QString PlayListGalleryItem::group() const
{
    return QString::number(m_record->type());
}
