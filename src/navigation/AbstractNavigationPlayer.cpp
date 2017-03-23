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

void AbstractNavigationPlayer::onEnter()
{
}

void AbstractNavigationPlayer::onLeave()
{
}

void AbstractNavigationPlayer::reset()
{
}

QDialog *AbstractNavigationPlayer::createConfigureDialog()
{
    return nullptr;
}

bool AbstractNavigationPlayer::isConfigurable() const
{
    return false;
}
