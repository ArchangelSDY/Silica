#include "PlayListProviderManager.h"

#include "db/LocalDatabase.h"
#include "playlist/LocalPlayListProviderFactory.h"
#include "playlist/PlayListProvider.h"
#include "playlist/PlayListProviderFactory.h"
#include "playlist/PlayListRecord.h"
#include "sapi/IPlayListProviderPlugin.h"
#include "sapi/PlayListProviderFactoryDelegate.h"
#include "sapi/PluginLoader.h"
#include "GlobalConfig.h"

PlayListProviderManager *PlayListProviderManager::s_instance = 0;

PlayListProviderManager *PlayListProviderManager::instance()
{
    if (!s_instance) {
        s_instance = new PlayListProviderManager();
    }
    return s_instance;
}

PlayListProviderManager::PlayListProviderManager()
{
    // Register plugin providers
    sapi::PluginLoadCallback<sapi::IPlayListProviderPlugin> callback = [this](sapi::IPlayListProviderPlugin *plugin, const QJsonObject &meta) {
        QString name = meta["name"].toString();

        PlayListProviderFactory *factory =
            new sapi::PlayListProviderFactoryDelegate(plugin, meta);
        this->registerPluginProvider(name, factory);
    };

    sapi::loadPlugins("playlistproviders", callback);

    // Register internal providers
    registerProvider(LocalPlayListProviderFactory::TYPE,
                     new LocalPlayListProviderFactory());
}

PlayListProviderManager::~PlayListProviderManager()
{
    qDeleteAll(m_providers.begin(), m_providers.end());
}

PlayListProvider *PlayListProviderManager::create(int type)
{
    if (!m_providers.contains(type)) {
        return 0;
    }

    PlayListProviderFactory *factory = m_providers.value(type);
    PlayListProvider *provider = factory->create();

    return provider;
}

QList<int> PlayListProviderManager::registeredTypes() const
{
    return m_providers.keys();
}

void PlayListProviderManager::registerPluginProvider(const QString &name,
                                               PlayListProviderFactory *factory)
{
    if (name.isEmpty()) {
        return;
    }

    int typeId = LocalDatabase::instance()->queryPluginPlayListProviderType(name);
    if (typeId == PlayListRecord::UNKNOWN_TYPE) {
        typeId = LocalDatabase::instance()->insertPluginPlayListProviderType(name);
    }

    registerProvider(typeId, factory);
}

void PlayListProviderManager::registerProvider(int typeId,
        PlayListProviderFactory *factory)
{
    m_providers.insert(typeId, factory);
}

