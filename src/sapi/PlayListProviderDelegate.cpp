#include "PlayListProviderDelegate.h"

#include "sapi/IPlayListProviderPlugin.h"

namespace sapi {

PlayListProviderDelegate::PlayListProviderDelegate(
        IPlayListProviderPlugin *plugin, IPlayListProvider *provider) :
    m_plugin(plugin) ,
    m_provider(provider)
{
    connect(m_provider, SIGNAL(gotItems(QList<QUrl>,QList<QVariantHash>)),
            this, SIGNAL(gotItems(QList<QUrl>,QList<QVariantHash>)));
}

PlayListProviderDelegate::~PlayListProviderDelegate()
{
    disconnect(m_provider, SIGNAL(gotItems(QList<QUrl>,QList<QVariantHash>)),
               this, SIGNAL(gotItems(QList<QUrl>,QList<QVariantHash>)));
    m_plugin->destroy(m_provider);
}

QString PlayListProviderDelegate::typeName() const
{
    // FIXME(sdy)
    return "plugin";
}

void PlayListProviderDelegate::request(const QString &name,
                                       const QVariantHash &extras)
{
    return m_provider->request(name, extras);
}

}

