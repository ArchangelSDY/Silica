#pragma once

#include "playlist/PlayListEntity.h"

namespace sapi {

class PlayListProviderDelegate;

class PlayListEntityDelegate : public PlayListEntity
{
public:
    PlayListEntityDelegate(PlayListProviderDelegate *provider,
        const QString &name, int count, const QString &coverPath, bool canContinueProvide);

    virtual ~PlayListEntityDelegate() override;
    virtual int count() const override;
    virtual QString name() const override;
    virtual bool supportsOption(PlayListEntityOption option) const override;
    virtual QImage loadCoverImage() override;
    virtual QList<QUrl> loadImageUrls() override;

    virtual void setCoverImagePath(const QString &path) override;

private:
    QString m_name;
    int m_count;
    QString m_coverPath;
    bool m_canContinueProvide;
};

}
