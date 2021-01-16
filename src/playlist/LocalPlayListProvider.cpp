#include "LocalPlayListProvider.h"

#include <QList>
#include <QUrl>

#include "db/LocalDatabase.h"
#include "playlist/LocalPlayListEntity.h"
#include "playlist/LocalPlayListProviderFactory.h"

LocalPlayListProvider::LocalPlayListProvider(LocalDatabase *db, QObject *parent) :
    PlayListProvider(parent) ,
    m_db(db)
{
}

LocalPlayListProvider::~LocalPlayListProvider()
{
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

QList<PlayListEntity *> LocalPlayListProvider::loadEntities()
{
    QList<PlayListEntity *> entities;

    auto items = m_db->queryPlayListEntities(LocalPlayListProviderFactory::TYPE);
    for (const auto& item : items) {
        entities << new LocalPlayListEntity(this, item.id, item.name, item.count, item.coverPath);
    }

    return entities;
}

PlayListEntityTriggerResult LocalPlayListProvider::triggerEntity(PlayListEntity *entity)
{
    return PlayListEntityTriggerResult::LoadPlayList;
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
    data.coverPath = e->m_coverPath;
    m_db->insertPlayListEntity(data);
    e->m_id = data.id;

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
    m_db->updatePlayListEntity(data);

    emit entitiesChanged();
}

void LocalPlayListProvider::removeEntity(PlayListEntity *entity)
{
    LocalPlayListEntity *e = static_cast<LocalPlayListEntity *>(entity);

    m_db->removePlayListEntity(e->m_id);

    emit entitiesChanged();
}
