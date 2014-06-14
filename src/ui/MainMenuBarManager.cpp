#include "ExpandingNavigationPlayer.h"
#include "HotspotsNavigationPlayer.h"
#include "MainMenuBarManager.h"
#include "Navigator.h"
#include "NormalNavigationPlayer.h"

MainMenuBarManager::MainMenuBarManager(Context context, QObject *parent) :
    QObject(parent) ,
    m_menuBar(context.menuBar) ,
    m_navigator(context.navigator) ,
    m_imageView(context.imageView)
{
    init();
}

void MainMenuBarManager::init()
{
    QMenu *playersMenu = m_menuBar->addMenu(tr("Players"));

    QActionGroup *playersGrp = new QActionGroup(playersMenu);
    playersGrp->setExclusive(true);

    QAction *actSetNormalPlayer = playersMenu->addAction(
        tr("Normal Player"), this, SLOT(setNormalPlayer()));
    playersGrp->addAction(actSetNormalPlayer);
    actSetNormalPlayer->setCheckable(true);
    actSetNormalPlayer->setChecked(
        m_navigator->player()->type() == AbstractNavigationPlayer::NormalType);

    QAction *actSetHotspotsPlayer = playersMenu->addAction(
        tr("Hotspots Player"), this, SLOT(setHotspotsPlayer()));
    playersGrp->addAction(actSetHotspotsPlayer);
    actSetHotspotsPlayer->setCheckable(true);
    actSetHotspotsPlayer->setChecked(
        m_navigator->player()->type() == AbstractNavigationPlayer::HotspotsType);

    QAction *actSetExpandingPlayer = playersMenu->addAction(
        tr("Expanding Player"), this, SLOT(setExpandingPlayer()));
    playersGrp->addAction(actSetExpandingPlayer);
    actSetExpandingPlayer->setCheckable(true);
    actSetExpandingPlayer->setChecked(
        m_navigator->player()->type() == AbstractNavigationPlayer::ExpandingType);
}

void MainMenuBarManager::setNormalPlayer()
{
    m_navigator->setPlayer(new NormalNavigationPlayer(m_navigator));
}

void MainMenuBarManager::setHotspotsPlayer()
{
    m_navigator->setPlayer(new HotspotsNavigationPlayer(m_navigator));
}

void MainMenuBarManager::setExpandingPlayer()
{
    m_navigator->setPlayer(
        new ExpandingNavigationPlayer(m_navigator, m_imageView));
}
