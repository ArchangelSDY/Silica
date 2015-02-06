#ifndef PLAYLISTPROVIDERDELEGATE_H
#define PLAYLISTPROVIDERDELEGATE_H

#include "playlist/PlayListProvider.h"

namespace sapi {

class IPlayListProvider;
class IPlayListProviderPlugin;

class PlayListProviderDelegate : public PlayListProvider
{
public:
    PlayListProviderDelegate(IPlayListProviderPlugin *factory,
                             IPlayListProvider *provider,
                             const QString &name);
    ~PlayListProviderDelegate();

    // PlayListProvider interface
    QString typeName() const override;
    void request(const QString &name, const QVariantHash &extra) override;


private:
    IPlayListProviderPlugin *m_plugin;
    IPlayListProvider *m_provider;
    QString m_name;
};

}

#endif // PLAYLISTPROVIDERDELEGATE_H
