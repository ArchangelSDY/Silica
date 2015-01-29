#include "AbstractNavigationPlayer.h"

AbstractNavigationPlayer::AbstractNavigationPlayer(Navigator *navigator,
                                                   QObject *parent) :
    QObject(parent) ,
    m_navigator(navigator)
{
}

AbstractNavigationPlayer::~AbstractNavigationPlayer()
{
}

void AbstractNavigationPlayer::reset()
{
}

QDialog *AbstractNavigationPlayer::configureDialog() const
{
    return 0;
}

bool AbstractNavigationPlayer::isConfigurable() const
{
    return configureDialog() != 0;
}
