#pragma once

#include <QList>
#include <QObject>
#include <QVariantHash>

#include "image/Image.h"
#include "playlist/PlayListEntityTriggerResult.h"
#include "playlist/PlayListProviderOption.h"

class PlayListEntity;
class PlayListProviderFactory;

class PlayListProvider : public QObject
{
    Q_OBJECT
public:
    explicit PlayListProvider(QObject *parent = 0) : QObject(parent) {}

    virtual ~PlayListProvider() {}
    virtual int type() const = 0;
    virtual QString name() const = 0;
    virtual bool supportsOption(PlayListProviderOption option) const = 0;
    virtual QList<PlayListEntity *> loadEntities() = 0;
    virtual PlayListEntityTriggerResult triggerEntity(PlayListEntity *entity) = 0;
    virtual PlayListEntity *createEntity(const QString &name) = 0;
    virtual void insertEntity(PlayListEntity *entity) = 0;
    virtual void updateEntity(PlayListEntity *entity) = 0;
    virtual void removeEntity(PlayListEntity *entity) = 0;

signals:
    void entitiesChanged();
};
