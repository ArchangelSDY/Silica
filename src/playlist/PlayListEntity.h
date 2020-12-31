#pragma once

#include <QImage>
#include <QString>

#include "playlist/PlayListEntityOption.h"

class PlayListEntity
{
public:
    virtual ~PlayListEntity() {}
    virtual int count() const = 0;
    virtual QString name() const = 0;
    virtual bool supportsOption(PlayListEntityOption option) const = 0;
    virtual QImage loadCoverImage() = 0;

    // Load image urls in this playlist
    //
    // Note that we should not create PlayList directly here as PlayList must
    // be created on main thread so we can correctly receive signals there
    virtual QList<QUrl> loadImageUrls() = 0;
};
