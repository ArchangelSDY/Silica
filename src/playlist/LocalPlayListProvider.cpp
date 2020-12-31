#include "LocalPlayListProvider.h"

#include <QList>
#include <QUrl>

#include "db/LocalDatabase.h"
#include "playlist/LocalPlayListEntity.h"
#include "playlist/LocalPlayListProviderFactory.h"

LocalPlayListProvider::LocalPlayListProvider(QObject *parent) :
    PlayListProvider(parent)
{
}

LocalPlayListProvider::~LocalPlayListProvider()
{
    qDeleteAll(m_entities.begin(), m_entities.end());
}

int LocalPlayListProvider::type() const
{
    return LocalPlayListProvider::TYPE;
}

QString LocalPlayListProvider::name() const
{
    return QObject::tr("Local");
}

bool LocalPlayListProvider::supportsOption(PlayListProviderOption option) const
{
    return option == PlayListProviderOption::CreateEntity
        || option == PlayListProviderOption::UpdateEntity
        || option == PlayListProviderOption::RemoveEntity;
}

QList<PlayListEntity*> LocalPlayListProvider::entities() const
{
    QList<PlayListEntity*> ret;
    ret.reserve(m_entities.count());
    for (auto entity : m_entities) {
        ret.push_back(entity);
    }
    return ret;
}

void LocalPlayListProvider::loadEntities()
{
    qDeleteAll(m_entities.begin(), m_entities.end());
    auto items = LocalDatabase::instance()->queryPlayListEntities(LocalPlayListProviderFactory::TYPE);
    for (const auto& item : items) {
        m_entities << new LocalPlayListEntity(this, item.id, item.name, item.count, item.coverPath);
    }
    emit entitiesChanged();
}

void LocalPlayListProvider::triggerEntity(PlayListEntity *entity)
{
    emit playListTriggered(entity);
}

PlayListEntity *LocalPlayListProvider::createEntity(const QString &name)
{
    return new LocalPlayListEntity(this, {}, name, 0, {});
}

void LocalPlayListProvider::insertEntity(PlayListEntity *entity)
{
    LocalPlayListEntity *e = static_cast<LocalPlayListEntity *>(entity);

    PlayListEntityData data;
    data.name = e->m_name;
    data.type = LocalPlayListProvider::TYPE;
    LocalDatabase::instance()->insertPlayListEntity(data);
    e->m_id = data.id;

    m_entities << e;

    emit entitiesChanged();
}

void LocalPlayListProvider::updateEntity(PlayListEntity *entity)
{
    LocalPlayListEntity *e = static_cast<LocalPlayListEntity *>(entity);

    PlayListEntityData data;
    data.id = e->m_id;
    data.name = e->m_name;
    data.coverPath = e->m_coverPath;
    data.count = e->m_count;
    LocalDatabase::instance()->updatePlayListEntity(data);

    emit entitiesChanged();
}

void LocalPlayListProvider::removeEntity(PlayListEntity *entity)
{
    LocalPlayListEntity *e = static_cast<LocalPlayListEntity *>(entity);

    PlayListEntityData data;
    data.id = e->m_id;
    LocalDatabase::instance()->removePlayListEntity(data);

    m_entities.removeOne(e);

    emit entitiesChanged();
}

// bool LocalPlayListProvider::canContinueProvide() const
// {
//     return false;
// }
// 
// void LocalPlayListProvider::request(const QString &name,
//                                     const QVariantHash &extra)
// {
//     int id = extra.value("id").toInt();
//     QList<QUrl> imageUrls =
//         LocalDatabase::instance()->queryImageUrlsForLocalPlayListRecord(id);
//     QList<QVariantHash> extraInfos;
// 
//     emit gotItems(imageUrls, extraInfos);
// }
// 
// bool LocalPlayListProvider::isImagesReadOnly() const
// {
//     return false;
// }
// 
// bool LocalPlayListProvider::insertImages(const PlayListRecord &record,
//                                          const ImageList &images)
// {
//     return LocalDatabase::instance()->insertImagesForLocalPlayListProvider(
//         record, images);
// }
// 
// bool LocalPlayListProvider::removeImages(const PlayListRecord &record,
//                                          const ImageList &images)
// {
//     return LocalDatabase::instance()->removeImagesForLocalPlayListProvider(
//         record, images);
// }
