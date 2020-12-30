#pragma once

#include <QImage>
#include <QString>

#include "playlist/PlayListEntityOption.h"

class PlayList;

class PlayListEntity
{
public:
    virtual ~PlayListEntity() {}
    virtual int count() const = 0;
    virtual QString name() const = 0;
    virtual bool supportsOption(PlayListEntityOption option) const = 0;
    virtual QImage loadCoverImage() = 0;
    virtual PlayList *createPlayList() = 0;
};
