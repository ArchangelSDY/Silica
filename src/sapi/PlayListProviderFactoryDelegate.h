#ifndef PLAYLISTPROVIDERFACTORYDELEGATE_H
#define PLAYLISTPROVIDERFACTORYDELEGATE_H

#include <QString>

#include "playlist/PlayListProviderFactory.h"

namespace sapi {

class IPlayListProviderPlugin;

class PlayListProviderFactoryDelegate : public PlayListProviderFactory
{
public:
    PlayListProviderFactoryDelegate(IPlayListProviderPlugin *plugin,
                                    const QString &name);
    ~PlayListProviderFactoryDelegate();

    // PlayListProviderFactory interface
    PlayListProvider *create();

private:
    IPlayListProviderPlugin *m_plugin;
    QString m_name;
};

}

#endif // PLAYLISTPROVIDERFACTORYDELEGATE_H
