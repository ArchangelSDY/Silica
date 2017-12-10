#include "Navigator.h"
#include "NormalNavigationPlayer.h"

NormalNavigationPlayer::NormalNavigationPlayer(Navigator *navigator,
                                               QWidget *view,
                                               QObject *parent) :
    AbstractNavigationPlayer(navigator, parent)
{
}

void NormalNavigationPlayer::goNext()
{
    m_navigator->goIndexUntilSuccess(m_navigator->currentIndex() + m_stepSize, 1);
}

void NormalNavigationPlayer::goPrev()
{
    m_navigator->goIndexUntilSuccess(m_navigator->currentIndex() - m_stepSize, -1);
}

void NormalNavigationPlayer::onEnter()
{
    // Set focused rect to null to disable focusing
    m_navigator->focusOnRect(QRectF());
}
