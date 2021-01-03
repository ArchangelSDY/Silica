#pragma once

#include <QSet>
#include <QSharedPointer>

#include "../../PlayList.h"

class BasketModel
{
public:
    BasketModel();

    QSharedPointer<PlayList> playList() const;
    QSharedPointer<PlayList> takePlayList();

    void add(ImagePtr image);
    void clear();

private:
    QSharedPointer<PlayList> m_playList;
    QSet<QUrl> m_imageUrls;
};
