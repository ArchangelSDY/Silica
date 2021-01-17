#pragma once

#include <QUrl>
#include <QVariant>

#include "sapi_global.h"
#include "PlayListProviderOption.h"
#include "PlayListEntityTriggerResult.h"

namespace sapi {

class IPlayListEntity;

class SAPI_EXPORT IPlayListProvider : public QObject
{
    Q_OBJECT
public:
    virtual ~IPlayListProvider();

    virtual bool supportsOption(PlayListProviderOption option) const = 0;
    virtual QList<IPlayListEntity *> loadEntities() = 0;
    virtual PlayListEntityTriggerResult triggerEntity(IPlayListEntity *entity) = 0;
    virtual IPlayListEntity *createEntity(const QString &name) = 0;
    virtual void insertEntity(IPlayListEntity *entity) = 0;
    virtual void updateEntity(IPlayListEntity *entity) = 0;
    virtual void removeEntity(IPlayListEntity *entity) = 0;

signals:
    void entitiesChanged();
};

}
