#include "Navigator.h"
#include "NormalNavigationPlayer.h"

NormalNavigationPlayer::NormalNavigationPlayer(Navigator *navigator,
                                               QObject *parent) :
    AbstractNavigationPlayer(navigator, parent) ,
    m_stepSize(1)
{
}

QString NormalNavigationPlayer::name() const
{
    return "Normal Player";
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

void NormalNavigationPlayer::setStepSize(int stepSize)
{
    m_stepSize = stepSize;
}
