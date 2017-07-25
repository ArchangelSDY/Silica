#include "NavigatorSynchronizer.h"

#include "Navigator.h"

NavigatorSynchronizer::NavigatorSynchronizer(Navigator *primary, Navigator *secondary) :
    m_primary(primary) ,
    m_secondary(secondary) ,
    m_offset(1)
{
    connect(m_primary, &Navigator::playListChange, this, &NavigatorSynchronizer::onPlayListChange);
    connect(m_primary, &Navigator::navigationChange, this, &NavigatorSynchronizer::onNavigationChange);
}

int NavigatorSynchronizer::offset() const
{
    return m_offset;
}

void NavigatorSynchronizer::setOffset(int offset)
{
    m_offset = offset;
}

void NavigatorSynchronizer::onPlayListChange(QSharedPointer<PlayList> playlist)
{
    m_secondary->setPlayList(playlist);
}

void NavigatorSynchronizer::onNavigationChange(int index)
{
    m_secondary->goIndex(index + m_offset);
}
