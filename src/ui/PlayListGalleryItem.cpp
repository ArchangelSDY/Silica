#include <QDir>
#include <QGraphicsScene>
#include <QtConcurrent>

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
    createRenderer();

    connect(m_record, SIGNAL(saved()), this, SLOT(loadThumbnail()));
    connect(&m_thumbnailLoader, &QFutureWatcher<QImage>::finished, this, &PlayListGalleryItem::onThumbnailLoaded);
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

    QString coverFullPath = GlobalConfig::instance()->thumbnailPath() +
        "/" + m_record->coverPath();
    m_thumbnailLoader.setFuture(QtConcurrent::run([coverFullPath]() -> QSharedPointer<QImage> {
        QSharedPointer<QImage> thumbnail(new QImage(coverFullPath));
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
        m_record->name(), m_record->count()));
}

QString PlayListGalleryItem::name() const
{
    return m_record->name();
}

QRectF PlayListGalleryItem::boundingRect() const
{
    return QRectF(QPointF(0, 0), GlobalConfig::instance()->galleryItemSize());
}
