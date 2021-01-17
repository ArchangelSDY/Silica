#pragma once

#include "playlist/PlayListEntity.h"

namespace sapi {

class IPlayListEntity;
class PlayListProviderDelegate;

class PlayListEntityDelegate : public ::PlayListEntity
{
public:
    PlayListEntityDelegate(PlayListProviderDelegate *provider, IPlayListEntity *entity);

    virtual int count() const override;
    virtual QString name() const override;
    virtual bool supportsOption(::PlayListEntityOption option) const override;
    virtual QImage loadCoverImage() override;
    virtual QList<QUrl> loadImageUrls() override;

    virtual void setName(const QString &name) override;
    virtual void setCoverImagePath(const QString &path) override;
    virtual void addImageUrls(const QList<QUrl> &imageUrls) override;
    virtual void removeImageUrls(const QList<QUrl> &imageUrls) override;

    IPlayListEntity *entity() const;

private:
    IPlayListEntity *m_entity;
};

}
