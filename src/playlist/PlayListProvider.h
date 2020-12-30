#pragma once

#include <QList>
#include <QObject>
#include <QVariantHash>

#include "image/Image.h"

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
    virtual QList<PlayListEntity *> entities() const = 0;
    virtual void loadEntities() = 0;
    virtual void triggerEntity(PlayListEntity *entity) = 0;

    // virtual QString typeName() const = 0;
    // virtual bool canContinueProvide() const = 0;

    // virtual void request(const QString &name,
    //                      const QVariantHash &extra = QVariantHash()) = 0;

    // virtual bool isImagesReadOnly() const { return true; }
    // virtual bool insertImages(const PlayListRecord &record,
    //                           const ImageList &images) { return false; }
    // virtual bool removeImages(const PlayListRecord &record,
    //                           const ImageList &images) { return false; }

signals:
    void entitiesChanged();
    void playListTriggered(PlayListEntity *entity);
//     void gotItems(const QList<QUrl> &urls,
//                   const QList<QVariantHash> &extraInfos);
};
