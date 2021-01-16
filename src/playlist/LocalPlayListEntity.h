#pragma once

#include "playlist/LocalPlayListProvider.h"
#include "playlist/PlayListEntity.h"

class LocalPlayListEntity : public PlayListEntity
{
public:
    LocalPlayListEntity(LocalPlayListProvider *provider,
        int id, const QString& name, int count, const QString& coverPath);
    virtual ~LocalPlayListEntity() override;
    virtual int count() const override;
    virtual QString name() const override;
    virtual bool supportsOption(PlayListEntityOption option) const override;
    virtual QImage loadCoverImage() override;
    virtual QList<QUrl> loadImageUrls() override;
    virtual void setName(const QString &name) override;
    virtual void setCoverImagePath(const QString &path) override;
    virtual void addImageUrls(const QList<QUrl> &imageUrls) override;
    virtual void removeImageUrls(const QList<QUrl> &imageUrls) override;

    int id() const;

private:
    friend void LocalPlayListProvider::insertEntity(PlayListEntity *);
    friend void LocalPlayListProvider::updateEntity(PlayListEntity *);
    friend void LocalPlayListProvider::removeEntity(PlayListEntity *);

    int m_id;
    QString m_name;
    int m_count;
    QString m_coverPath;
};
