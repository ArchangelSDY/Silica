#include "PlayListProviderFactoryDelegate.h"

#include "sapi/IPlayListProviderPlugin.h"
#include "sapi/PlayListProviderDelegate.h"

namespace sapi {

PlayListProviderFactoryDelegate::PlayListProviderFactoryDelegate(
        IPlayListProviderPlugin *plugin, const QJsonObject &meta) :
    m_plugin(plugin) ,
    m_meta(meta)
{
}

PlayListProvider *PlayListProviderFactoryDelegate::create()
{
    return new PlayListProviderDelegate(
        m_plugin, m_plugin->create(),
        m_meta["name"].toString(),
        m_meta["displayName"].toString(),
        m_meta["canContinueProvide"].toBool(false));
}

}

