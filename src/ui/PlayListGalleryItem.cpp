#include <QDir>
#include <QGraphicsScene>
#include <QtConcurrent>

#include "CompactImageRenderer.h"
#include "CompactTitleRenderer.h"
#include "GlobalConfig.h"
#include "LooseImageRenderer.h"
#include "LooseImageBackgroundRenderer.h"
#include "PlayListGalleryItem.h"

PlayListGalleryItem::PlayListGalleryItem(PlayListEntity *entity,
                                         AbstractRendererFactory *rendererFactory,
                                         QGraphicsItem *parent) :
    GalleryItem(rendererFactory, parent) ,
    m_entity(entity)
{
    setFlag(QGraphicsItem::ItemIsSelectable);
    setToolTip(m_entity->name());
    createRenderer();

    // TODO
    // connect(m_record, SIGNAL(saved()), this, SLOT(loadThumbnail()));
    connect(&m_thumbnailLoader, &QFutureWatcher<QImage>::finished, this, &PlayListGalleryItem::onThumbnailLoaded);
}

PlayListGalleryItem::~PlayListGalleryItem()
{
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

    PlayListEntity *entity = m_entity;
    m_thumbnailLoader.setFuture(QtConcurrent::run([entity]() -> QSharedPointer<QImage> {
        QSharedPointer<QImage> thumbnail(new QImage(std::move(entity->loadCoverImage())));
        if (thumbnail->isNull()) {
            thumbnail->load(":/res/playlist.png");
        }
        return thumbnail;
    }));
}

void PlayListGalleryItem::onThumbnailLoaded()
{
    setThumbnail(m_thumbnailLoader.result());

    // Replace with new render since cover image has changed
    createRenderer();
}

void PlayListGalleryItem::createRenderer()
{
    setRenderer(m_rendererFactory->createItemRendererForPlayListGallery(
        m_entity->name(), m_entity->count()));
}

QString PlayListGalleryItem::name() const
{
    return m_entity->name();
}

QRectF PlayListGalleryItem::boundingRect() const
{
    return QRectF(QPointF(0, 0), GlobalConfig::instance()->galleryItemSize());
}
