#include "PlayListEntity.h"

PlayListEntity::PlayListEntity(PlayListProvider *provider) :
    m_provider(provider)
{
}

PlayListEntity::~PlayListEntity()
{
}

PlayListProvider *PlayListEntity::provider() const
{
    return m_provider;
}
