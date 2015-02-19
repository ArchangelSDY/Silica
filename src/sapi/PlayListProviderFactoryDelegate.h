#ifndef PLAYLISTPROVIDERFACTORYDELEGATE_H
#define PLAYLISTPROVIDERFACTORYDELEGATE_H

#include <QJsonObject>
#include <QString>

#include "playlist/PlayListProviderFactory.h"

namespace sapi {

class IPlayListProviderPlugin;

class PlayListProviderFactoryDelegate : public PlayListProviderFactory
{
public:
    PlayListProviderFactoryDelegate(IPlayListProviderPlugin *plugin,
                                    const QJsonObject &meta);
    ~PlayListProviderFactoryDelegate();

    // PlayListProviderFactory interface
    PlayListProvider *create();

private:
    IPlayListProviderPlugin *m_plugin;
    QJsonObject m_meta;
};

}

#endif // PLAYLISTPROVIDERFACTORYDELEGATE_H
