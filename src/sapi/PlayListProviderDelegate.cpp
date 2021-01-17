#include "PlayListProviderDelegate.h"

#include "db/LocalDatabase.h"
#include "sapi/IPlayListProviderPlugin.h"
#include "sapi/PlayListEntityDelegate.h"

namespace sapi {

PlayListProviderDelegate::PlayListProviderDelegate(
        IPlayListProviderPlugin *plugin, IPlayListProvider *provider,
        int type, const QString &name, const QString &displayName) :
    m_plugin(plugin) ,
    m_provider(provider) ,
    m_type(type) ,
    m_name(name) ,
    m_displayName(displayName)
{
    connect(m_provider, &IPlayListProvider::entitiesChanged, this, &PlayListProviderDelegate::entitiesChanged);
}

PlayListProviderDelegate::~PlayListProviderDelegate()
{
    disconnect(m_provider, &IPlayListProvider::entitiesChanged, this, &PlayListProviderDelegate::entitiesChanged);
    m_plugin->destroy(m_provider);
}

int PlayListProviderDelegate::type() const
{
    return m_type;
}

QString PlayListProviderDelegate::name() const
{
    return m_displayName;
}

bool PlayListProviderDelegate::supportsOption(::PlayListProviderOption option) const
{
    return m_provider->supportsOption((sapi::PlayListProviderOption)option);
}

QList<::PlayListEntity *> PlayListProviderDelegate::loadEntities()
{
    QList<::PlayListEntity *> delegates;

    auto entities = m_provider->loadEntities();
    delegates.reserve(entities.count());

    for (auto entity : entities) {
        delegates << new PlayListEntityDelegate(this, entity);
    }

    return delegates;
}

::PlayListEntityTriggerResult PlayListProviderDelegate::triggerEntity(::PlayListEntity *entity)
{
    auto ientity = static_cast<PlayListEntityDelegate *>(entity);
    return (::PlayListEntityTriggerResult)(m_provider->triggerEntity(ientity->entity()));
}

PlayListEntity *PlayListProviderDelegate::createEntity(const QString &name)
{
    return new PlayListEntityDelegate(this, m_provider->createEntity(name));
}

void PlayListProviderDelegate::insertEntity(PlayListEntity *entity)
{
    auto ientity = static_cast<PlayListEntityDelegate *>(entity);
    m_provider->insertEntity(ientity->entity());
}

void PlayListProviderDelegate::updateEntity(PlayListEntity *entity)
{
    auto ientity = static_cast<PlayListEntityDelegate *>(entity);
    m_provider->updateEntity(ientity->entity());
}

void PlayListProviderDelegate::removeEntity(PlayListEntity *entity)
{
    auto ientity = static_cast<PlayListEntityDelegate *>(entity);
    m_provider->removeEntity(ientity->entity());
}

}
