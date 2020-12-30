#pragma once

#include "playlist/PlayListProvider.h"

class LocalPlayListEntity;

class LocalPlayListProvider : public PlayListProvider
{
public:
    static const int TYPE = 0;

    LocalPlayListProvider(QObject *parent = nullptr);
    virtual ~LocalPlayListProvider() override;
    virtual int type() const override;
    virtual QString name() const override;
    virtual QList<PlayListEntity *> entities() const override;
    virtual void loadEntities() override;
    virtual void triggerEntity(PlayListEntity *entity) override;

    // QString typeName() const;
    // bool canContinueProvide() const;

    // void request(const QString &name, const QVariantHash &extra);

    // bool isImagesReadOnly() const;
    // bool insertImages(const PlayListRecord &record, const ImageList &images);
    // bool removeImages(const PlayListRecord &record, const ImageList &images);
private:
    QList<LocalPlayListEntity *> m_entities; 
};
