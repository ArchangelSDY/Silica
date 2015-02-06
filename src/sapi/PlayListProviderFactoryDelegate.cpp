#include "PlayListProviderFactoryDelegate.h"

#include "sapi/IPlayListProviderPlugin.h"
#include "sapi/PlayListProviderDelegate.h"

namespace sapi {

PlayListProviderFactoryDelegate::PlayListProviderFactoryDelegate(
        IPlayListProviderPlugin *plugin, const QString &name) :
    m_plugin(plugin) ,
    m_name(name)
{
}

PlayListProviderFactoryDelegate::~PlayListProviderFactoryDelegate()
{
    delete m_plugin;
}

PlayListProvider *PlayListProviderFactoryDelegate::create()
{
    return new PlayListProviderDelegate(m_plugin, m_plugin->create(), m_name);
}

}
