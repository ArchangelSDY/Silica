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
    // bool canContinueProvide() const override;

    virtual QList<PlayListEntity *> entities() const override;
    virtual void loadEntities() override;
    virtual void triggerEntity(PlayListEntity *entity) override;

    // void request(const QString &name, const QVariantHash &extra) override;


private:
    IPlayListProviderPlugin *m_plugin;
    IPlayListProvider *m_provider;
    int m_type;
    QString m_name;
    QString m_displayName;
    QList<PlayListEntityDelegate *> m_entities;
    bool m_canContinueProvide;
};

}
