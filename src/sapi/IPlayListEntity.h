#pragma once

#include <QUrl>
#include <QVariant>

#include "sapi_global.h"
#include "IPlayListEntityLoadContext.h"
#include "PlayListEntityOption.h"

namespace sapi {

class SAPI_EXPORT IPlayListEntity
{
public:
    virtual ~IPlayListEntity();

    virtual int count() const = 0;
    virtual QString name() const = 0;
    virtual bool supportsOption(PlayListEntityOption option) const = 0;
    virtual QImage loadCoverImage() = 0;
    virtual IPlayListEntityLoadContext *createLoadContext() = 0;
    virtual QList<QUrl> loadImageUrls(IPlayListEntityLoadContext *ctx) = 0;
    virtual void setName(const QString &name) = 0;
    virtual void setCoverImagePath(const QString &path) = 0;
    virtual void addImageUrls(const QList<QUrl> &imageUrls) = 0;
    virtual void removeImageUrls(const QList<QUrl> &imageUrls) = 0;
};

}
