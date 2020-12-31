#pragma once

#include "playlist/LocalPlayListProvider.h"
#include "playlist/PlayListEntity.h"

class LocalPlayListEntity : public PlayListEntity
{
public:
    LocalPlayListEntity(LocalPlayListProvider *provider,
        int type, int id, const QString& name, int count, const QString& coverPath);
    virtual ~LocalPlayListEntity() override;
    virtual int count() const override;
    virtual QString name() const override;
    virtual bool supportsOption(PlayListEntityOption option) const override;
    virtual QImage loadCoverImage() override;
    virtual QList<QUrl> loadImageUrls() override;
    virtual void setCoverImagePath(const QString &path) override;

private:
    friend void LocalPlayListProvider::updateEntity(PlayListEntity *);

    // TODO: It seems we don't need this
    int m_type;
    int m_id;
    QString m_name;
    int m_count;
    QString m_coverPath;
};
