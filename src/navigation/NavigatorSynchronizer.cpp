#include "NavigatorSynchronizer.h"

#include "Navigator.h"

NavigatorSynchronizer::NavigatorSynchronizer(Navigator *primary, Navigator *secondary) :
    m_primary(primary) ,
    m_secondary(secondary) ,
    m_offset(-1) ,
    m_enabled(false)
{
}

int NavigatorSynchronizer::offset() const
{
    return m_offset;
}

void NavigatorSynchronizer::setOffset(int offset)
{
    m_offset = offset;

    if (m_enabled) {
        onNavigationChange(m_primary->currentIndex());
    }
}

bool NavigatorSynchronizer::isEnabled() const
{
    return m_enabled;
}

void NavigatorSynchronizer::setEnabled(bool enabled)
{
    m_enabled = enabled;

    if (m_enabled) {
        connect(m_primary, &Navigator::playListChange, this, &NavigatorSynchronizer::onPlayListChange);
        connect(m_primary, &Navigator::navigationChange, this, &NavigatorSynchronizer::onNavigationChange);

        m_secondary->setPlayList(m_primary->playList());
        onNavigationChange(m_primary->currentIndex());
    } else {
        disconnect(m_primary, &Navigator::playListChange, this, &NavigatorSynchronizer::onPlayListChange);
        disconnect(m_primary, &Navigator::navigationChange, this, &NavigatorSynchronizer::onNavigationChange);
    }

    emit toggled(enabled);
}

void NavigatorSynchronizer::setLoop(bool loop)
{
    m_primary->setLoop(loop);
    m_secondary->setLoop(loop);
}

void NavigatorSynchronizer::onPlayListChange(QSharedPointer<PlayList> playlist)
{
    m_secondary->setPlayList(playlist);
}

void NavigatorSynchronizer::onNavigationChange(int index)
{
    int delta = m_offset > 0 ? 1 : -1;
    m_secondary->goIndexUntilSuccess(index + m_offset, delta);
}
