#include "AbstractNavigationPlayer.h"

AbstractNavigationPlayer::AbstractNavigationPlayer(Navigator *navigator,
                                                   QObject *parent) :
    QObject(parent) ,
    m_navigator(navigator)
{
}

void AbstractNavigationPlayer::reset()
{
}
