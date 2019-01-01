#include "AbstractNavigationPlayer.h"

#include <QMetaClassInfo>
#include <QMetaObject>

#include "Navigator.h"

AbstractNavigationPlayer::AbstractNavigationPlayer(Navigator *navigator,
                                                   QObject *parent) :
    QObject(parent) ,
    m_navigator(navigator) ,
    m_stepSize(1)
{
}

AbstractNavigationPlayer::~AbstractNavigationPlayer()
{
}

QString AbstractNavigationPlayer::name() const
{
    return this->metaObject()->classInfo(0).value();
}

QString AbstractNavigationPlayer::className() const
{
    return this->metaObject()->className();
}

void AbstractNavigationPlayer::goIndexUntilSuccess(int index, int delta)
{
    m_navigator->goIndexUntilSuccess(index, delta);
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

int AbstractNavigationPlayer::stepSize() const
{
    return m_stepSize;
}

void AbstractNavigationPlayer::setStepSize(int stepSize)
{
    m_stepSize = stepSize;
}

QDialog *AbstractNavigationPlayer::createConfigureDialog()
{
    return nullptr;
}

bool AbstractNavigationPlayer::isConfigurable() const
{
    return false;
}

void AbstractNavigationPlayer::cloneConfigurationFrom(AbstractNavigationPlayer *player)
{
}
