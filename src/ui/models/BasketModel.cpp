#include "BasketModel.h"

#include "image/ImageSource.h"

BasketModel::BasketModel() :
    m_playList(new PlayList())
{
}

QSharedPointer<PlayList> BasketModel::playList() const
{
    return m_playList;
}

QSharedPointer<PlayList> BasketModel::takePlayList()
{
    auto playList = m_playList;
    m_playList.reset(new PlayList());
    return playList;
}

void BasketModel::add(ImagePtr image)
{
    QUrl imageUrl = image->source()->url();
    if (!m_imageUrls.contains(imageUrl)) {
        *m_playList << image;
        m_imageUrls.insert(imageUrl);
    }
}

void BasketModel::clear()
{
    m_playList->clear();
    m_imageUrls.clear();
}
