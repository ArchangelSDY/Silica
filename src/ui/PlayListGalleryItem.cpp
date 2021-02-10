#include <QDir>
#include <QGraphicsScene>
#include <QtConcurrent>

#include "CompactImageRenderer.h"
#include "CompactTitleRenderer.h"
#include "GlobalConfig.h"
#include "LooseImageRenderer.h"
#include "LooseImageBackgroundRenderer.h"
#include "PlayListGalleryItem.h"

PlayListGalleryItem::PlayListGalleryItem(QSharedPointer<PlayListEntity> entity,
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

    auto entityRef = m_entity.toWeakRef();
    m_thumbnailLoader.setFuture(QtConcurrent::run([entityRef]() -> QSharedPointer<QImage> {
        auto entity = entityRef.toStrongRef();
        if (!entity) {
            return {};
        }
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
}

void PlayListGalleryItem::createRenderer()
{
    auto count = m_entity->supportsOption(PlayListEntityOption::Count) ?
        m_entity->count() :
        std::optional<int>();
    setRenderer(m_rendererFactory->createItemRendererForPlayListGallery(
        m_entity->name(), count));
}

QString PlayListGalleryItem::name() const
{
    return m_entity->name();
}

QRectF PlayListGalleryItem::boundingRect() const
{
    return QRectF(QPointF(0, 0), GlobalConfig::instance()->galleryItemSize());
}
