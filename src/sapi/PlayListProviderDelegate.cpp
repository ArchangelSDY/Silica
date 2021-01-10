#include "PlayListProviderDelegate.h"

#include "db/LocalDatabase.h"
#include "sapi/IPlayListProviderPlugin.h"
#include "sapi/PlayListEntityDelegate.h"

namespace sapi {

PlayListProviderDelegate::PlayListProviderDelegate(
        IPlayListProviderPlugin *plugin, IPlayListProvider *provider,
        int type, const QString &name, const QString &displayName, bool canContinueProvide) :
    m_plugin(plugin) ,
    m_provider(provider) ,
    m_type(type) ,
    m_name(name) ,
    m_displayName(displayName) ,
    m_canContinueProvide(canContinueProvide)
{
    // connect(m_provider, SIGNAL(gotItems(QList<QUrl>,QList<QVariantHash>)),
    //         this, SIGNAL(gotItems(QList<QUrl>,QList<QVariantHash>)));
    // connect(m_provider, SIGNAL(itemsCountChanged(int)),
    //         this, SIGNAL(itemsCountChanged(int)));
}

PlayListProviderDelegate::~PlayListProviderDelegate()
{
    // disconnect(m_provider, SIGNAL(gotItems(QList<QUrl>,QList<QVariantHash>)),
    //            this, SIGNAL(gotItems(QList<QUrl>,QList<QVariantHash>)));
    // disconnect(m_provider, SIGNAL(itemsCountChanged(int)),
    //            this, SIGNAL(itemsCountChanged(int)));
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

bool PlayListProviderDelegate::supportsOption(PlayListProviderOption option) const
{
    // TODO: Add extensibility
    return false;
}

QList<PlayListEntity *> PlayListProviderDelegate::loadEntities()
{
    QList<PlayListEntity *> entities;

    int type = LocalDatabase::instance()->queryPluginPlayListProviderType(m_name);
    // TODO: Replace with named constant
    if (type == -1) {
        return entities;
    }

    auto items = LocalDatabase::instance()->queryPlayListEntities(type);
    for (const auto &item : items) {
        entities << new PlayListEntityDelegate(this,
            item.name, item.count, item.coverPath, m_canContinueProvide);
    }

    return entities;
}

PlayListEntityTriggerResult PlayListProviderDelegate::triggerEntity(PlayListEntity *entity)
{
    // TODO: Add extensibility
    return PlayListEntityTriggerResult::LoadPlayList;
}

PlayListEntity *PlayListProviderDelegate::createEntity(const QString &name)
{
    // TODO: Add extensibility
    return nullptr;
}

void PlayListProviderDelegate::insertEntity(PlayListEntity *entity)
{
    // TODO: Add extensibility
}

void PlayListProviderDelegate::updateEntity(PlayListEntity *entity)
{
    // TODO: Add extensibility
}

void PlayListProviderDelegate::removeEntity(PlayListEntity *entity)
{
    // TODO: Add extensibility
}

// bool PlayListProviderDelegate::canContinueProvide() const
// {
//     return m_canContinueProvide;
// }

// void PlayListProviderDelegate::request(const QString &name,
//                                        const QVariantHash &extras)
// {
//     return m_provider->request(name, extras);
// }

}
