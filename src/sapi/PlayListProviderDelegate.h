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
                             const QString &name,
                             bool canContinueProvide);
    ~PlayListProviderDelegate();

    // PlayListProvider interface
    QString typeName() const override;
    bool canContinueProvide() const override;

    void request(const QString &name, const QVariantHash &extra) override;


private:
    IPlayListProviderPlugin *m_plugin;
    IPlayListProvider *m_provider;
    QString m_name;
    bool m_canContinueProvide;
};

}

#endif // PLAYLISTPROVIDERDELEGATE_H
