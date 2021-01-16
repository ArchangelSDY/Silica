#pragma once

#include "playlist/PlayListProvider.h"

class LocalDatabase;
class LocalPlayListEntity;

class LocalPlayListProvider : public PlayListProvider
{
public:
    static const int TYPE = 0;

    LocalPlayListProvider(LocalDatabase *db, QObject *parent = nullptr);
    virtual ~LocalPlayListProvider() override;
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
    LocalDatabase *m_db;
};
