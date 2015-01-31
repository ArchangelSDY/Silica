#include "PlayListProviderManager.h"

#include <QPluginLoader>

#include "GlobalConfig.h"
#include "PlayListProvider.h"
#include "PlayListProviderFactory.h"
#include "PlayListRecord.h"

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
    QDir pluginsDir(GlobalConfig::instance()->pluginsPath());
    if (pluginsDir.cd("playlistproviders")) {
        foreach (const QString &filename, pluginsDir.entryList(QDir::Files)) {
            QPluginLoader loader(pluginsDir.absoluteFilePath(filename));
            QObject *plugin = loader.instance();
            if (plugin) {
                PlayListProviderFactory *factory =
                    qobject_cast<PlayListProviderFactory *>(plugin);
                if (factory) {
                    // FIXME!!!
                    m_providers.insert(1, factory);
                }
            }
        }
    }
}

PlayListProviderManager::~PlayListProviderManager()
{
    qDeleteAll(m_providers.begin(), m_providers.end());
}

PlayListRecord *PlayListProviderManager::create(int type,
                                                int id, const QString &name,
                                                const QString &coverPath)
{
    if (!m_providers.contains(type)) {
        return 0;
    }

    PlayListProviderFactory *factory = m_providers.value(type);
    PlayListProvider *provider = factory->create();

    // TODO: Builder pattern
    PlayListRecord *record = new PlayListRecord(name, coverPath);
    record->setType(type);
    record->setId(id);

    QObject::connect(
        provider, SIGNAL(gotItems(QList<QUrl>,QList<QVariantHash>)),
        record, SLOT(gotItems(QList<QUrl>,QList<QVariantHash>)));

    QVariantHash extra;
    extra.insert("id", id);
    provider->request(name, extra);

    return record;
}

