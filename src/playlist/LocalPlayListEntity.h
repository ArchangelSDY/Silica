#pragma once

#include "playlist/PlayListEntity.h"

class LocalPlayListEntity : public PlayListEntity
{
public:
    LocalPlayListEntity(int type, int id, const QString& name, int count, const QString& coverPath);
    virtual ~LocalPlayListEntity() override;
    virtual int count() const override;
    virtual QString name() const override;
    virtual bool supportsOption(PlayListEntityOption option) const override;
    virtual QImage loadCoverImage() override;
    virtual QList<QUrl> loadImageUrls() override;

private:
    // TODO: It seems we don't need this
    int m_type;
    int m_id;
    QString m_name;
    int m_count;
    QString m_coverPath;
};
