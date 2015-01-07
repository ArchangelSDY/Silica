#include <QDir>
#include <QGraphicsScene>

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
    setRenderer(m_rendererFactory->createItemRendererForPlayListGallery(
        m_record->name(), m_record->count()));

    connect(m_record, SIGNAL(saved()), this, SLOT(loadThumbnail()));
}

PlayListGalleryItem::~PlayListGalleryItem()
{
    delete m_record;
}

void PlayListGalleryItem::load()
{
    loadThumbnail();
}

void PlayListGalleryItem::loadThumbnail()
{
    if (!m_rendererFactory) {
        return;
    }

    if (m_thumbnail) {
        delete m_thumbnail;
    }

    QString coverFullPath = GlobalConfig::instance()->thumbnailPath() +
        QDir::separator() + m_record->coverPath();
    QImage *thumbnail = new QImage(coverFullPath);
    if (thumbnail->isNull()) {
        thumbnail->load(":/res/playlist.png");
    }

    setThumbnail(thumbnail);

    // Replace with new render since cover image has changed
    setRenderer(m_rendererFactory->createItemRendererForPlayListGallery(
        m_record->name(), m_record->count()));
}

void PlayListGalleryItem::setRendererFactory(AbstractRendererFactory *factory)
{
    m_rendererFactory = factory;
    setRenderer(m_rendererFactory->createItemRendererForPlayListGallery(
        m_record->name(), m_record->count()));
}

QRectF PlayListGalleryItem::boundingRect() const
{
    return QRectF(QPointF(0, 0), GlobalConfig::instance()->galleryItemSize());
}
