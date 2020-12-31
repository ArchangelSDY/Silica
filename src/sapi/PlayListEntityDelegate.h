#pragma once

#include "playlist/PlayListEntity.h"

namespace sapi {

class IPlayListProvider;

class PlayListEntityDelegate : public PlayListEntity
{
public:
    PlayListEntityDelegate(IPlayListProvider *provider,
        const QString &name, int count, const QString &coverPath, bool canContinueProvide);

    virtual ~PlayListEntityDelegate() override;
    virtual int count() const override;
    virtual QString name() const override;
    virtual bool supportsOption(PlayListEntityOption option) const override;
    virtual QImage loadCoverImage() override;
    virtual QList<QUrl> loadImageUrls() override;

private:
    IPlayListProvider *m_provider;
    QString m_name;
    int m_count;
    QString m_coverPath;
    bool m_canContinueProvide;
};

}
