#pragma once

#include <QImage>
#include <QList>
#include <QString>
#include <QUrl>

#include "playlist/PlayListEntityOption.h"
#include "playlist/PlayListProvider.h"

class PlayListEntity
{
public:
    PlayListEntity(PlayListProvider *provider);
    PlayListProvider *provider() const;

    virtual ~PlayListEntity();
    virtual int count() const = 0;
    virtual QString name() const = 0;
    virtual bool supportsOption(PlayListEntityOption option) const = 0;
    virtual QImage loadCoverImage() = 0;

    // Load image urls in this playlist
    //
    // Note that we should not create PlayList directly here as PlayList must
    // be created on main thread so we can correctly receive signals there
    virtual QList<QUrl> loadImageUrls() = 0;

    virtual void setCoverImagePath(const QString &path) = 0;
    virtual void addImageUrls(const QList<QUrl> &imageUrls) = 0;
    virtual void removeImageUrls(const QList<QUrl> &imageUrls) = 0;

protected:
    PlayListProvider *m_provider;
};
