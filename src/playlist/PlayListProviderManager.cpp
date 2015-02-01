#include "PlayListProviderManager.h"

#include <QJsonObject>
#include <QJsonValue>
#include <QPluginLoader>

#include "db/LocalDatabase.h"
#include "playlist/LocalPlayListProviderFactory.h"
#include "playlist/PlayListProvider.h"
#include "playlist/PlayListProviderFactory.h"
#include "playlist/PlayListRecord.h"
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
    // Load plugins
    QDir pluginsDir(GlobalConfig::instance()->pluginsPath());
    if (pluginsDir.cd("playlistproviders")) {
        foreach (const QString &filename, pluginsDir.entryList(QDir::Files)) {
            QPluginLoader loader(pluginsDir.absoluteFilePath(filename));
            QObject *plugin = loader.instance();
            if (plugin) {
                PlayListProviderFactory *factory =
                    qobject_cast<PlayListProviderFactory *>(plugin);
                if (factory) {
                    QJsonObject meta = loader.metaData();
                    QJsonObject customMeta = meta["MetaData"].toObject();
                    QString name = customMeta["name"].toString();

                    registerPluginProvider(name, factory);
                }
            }
        }
    }

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

