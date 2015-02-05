#ifndef PLAYLISTPROVIDERFACTORYDELEGATE_H
#define PLAYLISTPROVIDERFACTORYDELEGATE_H

#include "playlist/PlayListProviderFactory.h"

namespace sapi {

class IPlayListProviderPlugin;

class PlayListProviderFactoryDelegate : public PlayListProviderFactory
{
public:
    PlayListProviderFactoryDelegate(IPlayListProviderPlugin *plugin);
    ~PlayListProviderFactoryDelegate();

    // PlayListProviderFactory interface
    PlayListProvider *create();

private:
    IPlayListProviderPlugin *m_plugin;

};

}

#endif // PLAYLISTPROVIDERFACTORYDELEGATE_H
