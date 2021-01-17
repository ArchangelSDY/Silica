#include "sapi/PlayListEntityDelegate.h"
#include <QHash>

#include "sapi/IPlayListEntity.h"
#include "sapi/PlayListProviderDelegate.h"
#include "../GlobalConfig.h"

namespace sapi {

PlayListEntityDelegate::PlayListEntityDelegate(sapi::PlayListProviderDelegate *provider, sapi::IPlayListEntity *entity) :
    PlayListEntity(provider) ,
    m_entity(entity)
{
}

int PlayListEntityDelegate::count() const
{
    return m_entity->count();
}

QString PlayListEntityDelegate::name() const
{
    return m_entity->name();
}

bool PlayListEntityDelegate::supportsOption(::PlayListEntityOption option) const
{
    return m_entity->supportsOption((sapi::PlayListEntityOption)option);
}

QImage PlayListEntityDelegate::loadCoverImage()
{
    return m_entity->loadCoverImage();
}

QList<QUrl> PlayListEntityDelegate::loadImageUrls()
{
    return m_entity->loadImageUrls();
}

void PlayListEntityDelegate::setName(const QString &name)
{
    m_entity->setName(name);
}

void PlayListEntityDelegate::setCoverImagePath(const QString &path)
{
    m_entity->setCoverImagePath(path);
}

void PlayListEntityDelegate::addImageUrls(const QList<QUrl> &imageUrls)
{
    m_entity->addImageUrls(imageUrls);
}

void PlayListEntityDelegate::removeImageUrls(const QList<QUrl> &imageUrls)
{
    m_entity->removeImageUrls(imageUrls);
}

IPlayListEntity *PlayListEntityDelegate::entity() const
{
    return m_entity;
}

}
