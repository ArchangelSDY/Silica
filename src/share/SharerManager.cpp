#include "SharerManager.h"

#include <QObject>

#include "sapi/ISharerPlugin.h"
#include "sapi/PluginLoader.h"
#include "sapi/SharerPluginDelegate.h"
#include "share/SaveToSharer.h"

SharerManager *SharerManager::s_instance = nullptr;

SharerManager *SharerManager::instance()
{
    if (!s_instance) {
        s_instance = new SharerManager();
    }
    return s_instance;
}

SharerManager::SharerManager()
{
    // Register built-ins
    registerSharer(QObject::tr("Save To"), new SaveToSharer());

    // Register plugins
    sapi::PluginLoadCallback<sapi::ISharerPlugin> callback = [this](sapi::ISharerPlugin *plugin, const QJsonObject &meta) {
        QString name = meta["displayName"].toString();
        sapi::SharerPluginDelegate *sharer = new sapi::SharerPluginDelegate(plugin);
        this->registerSharer(name, sharer);
    };
    sapi::loadPlugins("sharers", callback);
}

SharerManager::~SharerManager()
{
    for (Sharer *sharer : m_sharers) {
        delete sharer;
    }
}

void SharerManager::registerSharer(const QString &name, Sharer *sharer)
{
    m_sharerNames << name;
    m_sharers << sharer;
}

QStringList SharerManager::sharerNames() const
{
    return m_sharerNames;
}

bool SharerManager::share(int index, const QList<QSharedPointer<Image>> &images)
{
    return m_sharers[index]->share(images);
}
