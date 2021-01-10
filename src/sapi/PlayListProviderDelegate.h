#pragma once

#include "playlist/PlayListProvider.h"

namespace sapi {

class IPlayListProvider;
class IPlayListProviderPlugin;
class PlayListEntityDelegate;

class PlayListProviderDelegate : public PlayListProvider
{
public:
    PlayListProviderDelegate(IPlayListProviderPlugin *factory,
                             IPlayListProvider *provider,
                             int type,
                             const QString &name,
                             const QString &displayName,
                             bool canContinueProvide);
    virtual ~PlayListProviderDelegate() override;

    virtual int type() const override;
    virtual QString name() const override;
    virtual bool supportsOption(PlayListProviderOption option) const override;

    virtual QList<PlayListEntity *> loadEntities() override;
    virtual PlayListEntityTriggerResult triggerEntity(PlayListEntity *entity) override;
    virtual PlayListEntity *createEntity(const QString &name) override;
    virtual void insertEntity(PlayListEntity *entity) override;
    virtual void updateEntity(PlayListEntity *entity) override;
    virtual void removeEntity(PlayListEntity *entity) override;

private:
    IPlayListProviderPlugin *m_plugin;
    IPlayListProvider *m_provider;
    int m_type;
    QString m_name;
    QString m_displayName;
    bool m_canContinueProvide;
};

}
