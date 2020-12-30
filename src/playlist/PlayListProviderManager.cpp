#include "PlayListProviderManager.h"

#include "db/LocalDatabase.h"
#include "playlist/LocalPlayListProvider.h"
#include "playlist/PlayListProvider.h"
#include "sapi/IPlayListProviderPlugin.h"
#include "sapi/PlayListProviderDelegate.h"
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
        if (name.isEmpty()) {
            return;
        }

        int typeId = LocalDatabase::instance()->queryPluginPlayListProviderType(name);
        // TODO: Replace with named constant
        if (typeId == -1) {
            typeId = LocalDatabase::instance()->insertPluginPlayListProviderType(name);
        }

        auto provider = new sapi::PlayListProviderDelegate(
			plugin, plugin->create(),
            typeId,
			meta["name"].toString(),
			meta["displayName"].toString(),
			meta["canContinueProvide"].toBool(false));
        this->registerProvider(typeId, provider);
    };

    sapi::loadPlugins("playlistproviders", callback);

    // Register internal providers
    registerProvider(LocalPlayListProvider::TYPE, new LocalPlayListProvider());
}

PlayListProviderManager::~PlayListProviderManager()
{
    qDeleteAll(m_providers.begin(), m_providers.end());
}

PlayListProvider *PlayListProviderManager::get(int type)
{
    return m_providers[type];
}

QList<PlayListProvider *> PlayListProviderManager::all() const
{
    return m_providers.values();
}

void PlayListProviderManager::registerProvider(int type, PlayListProvider *provider)
{
    m_providers.insert(type, provider);
}
