#include "PlayListProviderDelegate.h"

#include "sapi/IPlayListProviderPlugin.h"

namespace sapi {

PlayListProviderDelegate::PlayListProviderDelegate(
        IPlayListProviderPlugin *plugin, IPlayListProvider *provider,
        const QString &name, bool canContinueProvide) :
    m_plugin(plugin) ,
    m_provider(provider) ,
    m_name(name) ,
    m_canContinueProvide(canContinueProvide)
{
    connect(m_provider, SIGNAL(gotItems(QList<QUrl>,QList<QVariantHash>)),
            this, SIGNAL(gotItems(QList<QUrl>,QList<QVariantHash>)));
    connect(m_provider, SIGNAL(itemsCountChanged(int)),
            this, SIGNAL(itemsCountChanged(int)));
}

PlayListProviderDelegate::~PlayListProviderDelegate()
{
    disconnect(m_provider, SIGNAL(gotItems(QList<QUrl>,QList<QVariantHash>)),
               this, SIGNAL(gotItems(QList<QUrl>,QList<QVariantHash>)));
    disconnect(m_provider, SIGNAL(itemsCountChanged(int)),
               this, SIGNAL(itemsCountChanged(int)));
    m_plugin->destroy(m_provider);
}

QString PlayListProviderDelegate::typeName() const
{
    return m_name;
}

bool PlayListProviderDelegate::canContinueProvide() const
{
    return m_canContinueProvide;
}

void PlayListProviderDelegate::request(const QString &name,
                                       const QVariantHash &extras)
{
    return m_provider->request(name, extras);
}

}

