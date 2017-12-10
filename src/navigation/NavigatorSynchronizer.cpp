#include "NavigatorSynchronizer.h"

#include "navigation/AbstractNavigationPlayer.h"
#include "Navigator.h"

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
    // Assume it goes next
    int delta = 1;

    // In this very specific case, we can assume it goes prev
    // Currently it is a workaround until we have a way to know whether it goes prev or next
    // <primary_new = index>, <secondary_new>, <primary_old>, <secondary_old = m_secondaru->currentIndex()>
    if (index == m_secondary->currentIndex() - 3) {
        delta = -1;
    }

    m_secondary->player()->goIndexUntilSuccess(index + 1, delta);
}
