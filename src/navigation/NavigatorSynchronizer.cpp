#include "NavigatorSynchronizer.h"

#include "Navigator.h"

int NavigatorSynchronizer::OFFSET = 1;

NavigatorSynchronizer::NavigatorSynchronizer(Navigator *primary, Navigator *secondary) :
    m_primary(primary) ,
    m_secondary(secondary) ,
    m_enabled(false)
{
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
    int delta = OFFSET > 0 ? 1 : -1;
    m_secondary->goIndexUntilSuccess(index + OFFSET, delta);
}
