#pragma once

#include "playlist/LocalPlayListProvider.h"
#include "playlist/PlayListEntity.h"

class LocalDatabase;

class LocalPlayListEntity : public PlayListEntity
{
public:
    LocalPlayListEntity(LocalPlayListProvider *provider, LocalDatabase *db,
        int id, const QString& name, int count, const QString& coverPath);
    virtual ~LocalPlayListEntity() override;
    virtual int count() const override;
    virtual QString name() const override;
    virtual bool supportsOption(PlayListEntityOption option) const override;
    virtual QImage loadCoverImage() override;
    virtual PlayListEntityLoadContext *createLoadContext();
    virtual QList<QUrl> loadImageUrls(PlayListEntityLoadContext *ctx) override;
    virtual void setName(const QString &name) override;
    virtual void setCoverImagePath(const QString &path) override;
    virtual void addImageUrls(const QList<QUrl> &imageUrls) override;
    virtual void removeImageUrls(const QList<QUrl> &imageUrls) override;

    int id() const;
    QString coverImagePath() const;

private:
    friend void LocalPlayListProvider::insertEntity(PlayListEntity *);
    friend void LocalPlayListProvider::updateEntity(PlayListEntity *);
    friend void LocalPlayListProvider::removeEntity(PlayListEntity *);

    LocalDatabase *m_db;
    int m_id;
    QString m_name;
    int m_count;
    QString m_coverPath;
};
