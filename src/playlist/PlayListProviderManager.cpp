#include "PlayListProviderManager.h"

#include <QJsonObject>
#include <QJsonValue>
#include <QPluginLoader>

#include "db/LocalDatabase.h"
#include "playlist/LocalPlayListProviderFactory.h"
#include "playlist/PlayListProvider.h"
#include "playlist/PlayListProviderFactory.h"
#include "playlist/PlayListRecord.h"
#include "sapi/IPlayListProviderPlugin.h"
#include "sapi/PlayListProviderFactoryDelegate.h"
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
    foreach (const QString &libPath, qApp->libraryPaths()) {
        QDir libDir(libPath);
        if (!libDir.cd("playlistproviders")) {
            continue;
        }

        foreach (const QString &filename, libDir.entryList(QDir::Files)) {
            QPluginLoader loader(libDir.absoluteFilePath(filename));
            QObject *instance = loader.instance();
            if (instance) {
                sapi::IPlayListProviderPlugin *plugin =
                    qobject_cast<sapi::IPlayListProviderPlugin *>(instance);
                if (plugin) {
                    QJsonObject meta = loader.metaData();
                    QJsonObject customMeta = meta["MetaData"].toObject();
                    QString name = customMeta["name"].toString();

                    PlayListProviderFactory *factory =
                        new sapi::PlayListProviderFactoryDelegate(plugin,
                                                                  customMeta);
                    registerPluginProvider(name, factory);
                }
            } else {
                qWarning() << "[Plugin]" << loader.fileName()
                    << loader.errorString();
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

