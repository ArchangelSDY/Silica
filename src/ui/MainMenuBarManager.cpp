#include "MainMenuBarManager.h"

#include <QDialog>

#include "navigation/CascadeClassifierNavigationPlayer.h"
#include "navigation/ExpandingNavigationPlayer.h"
#include "navigation/FixedRegionNavigationPlayer.h"
#include "navigation/HotspotsNavigationPlayer.h"
#include "navigation/NormalNavigationPlayer.h"
#include "ui/PluginLogsDialog.h"
#include "Navigator.h"

MainMenuBarManager::MainMenuBarManager(Context context, QObject *parent) :
    QObject(parent) ,
    m_menuBar(context.menuBar) ,
    m_navigator(context.navigator) ,
    m_imageView(context.imageView) ,
    m_menuPlayers(0) ,
    m_actPlayerConf(0) ,
    m_menuTools(0) ,
    m_pluginLogsDialog(new PluginLogsDialog())
{
    init();
}

MainMenuBarManager::~MainMenuBarManager()
{
    delete m_pluginLogsDialog;
}

void MainMenuBarManager::init()
{
    m_menuPlayers = m_menuBar->addMenu(tr("Players"));

    QActionGroup *playersGrp = new QActionGroup(m_menuPlayers);
    playersGrp->setExclusive(true);

    QAction *actSetNormalPlayer = m_menuPlayers->addAction(
        tr("Normal Player"), this, SLOT(setNormalPlayer()));
    playersGrp->addAction(actSetNormalPlayer);
    actSetNormalPlayer->setCheckable(true);
    actSetNormalPlayer->setChecked(
        m_navigator->player()->type() == AbstractNavigationPlayer::NormalType);

    QAction *actSetHotspotsPlayer = m_menuPlayers->addAction(
        tr("Hotspots Player"), this, SLOT(setHotspotsPlayer()));
    playersGrp->addAction(actSetHotspotsPlayer);
    actSetHotspotsPlayer->setCheckable(true);
    actSetHotspotsPlayer->setChecked(
        m_navigator->player()->type() == AbstractNavigationPlayer::HotspotsType);

    QAction *actSetExpandingPlayer = m_menuPlayers->addAction(
        tr("Expanding Player"), this, SLOT(setExpandingPlayer()));
    playersGrp->addAction(actSetExpandingPlayer);
    actSetExpandingPlayer->setCheckable(true);
    actSetExpandingPlayer->setChecked(
        m_navigator->player()->type() == AbstractNavigationPlayer::ExpandingType);

    QAction *actSetFixedRegionPlayer = m_menuPlayers->addAction(
        tr("Fixed Region Player"), this, SLOT(setFixedRegionPlayer()));
    playersGrp->addAction(actSetFixedRegionPlayer);
    actSetFixedRegionPlayer->setCheckable(true);
    actSetFixedRegionPlayer->setChecked(
        m_navigator->player()->type() == AbstractNavigationPlayer::FixedRegionType);

    QAction *actSetCascadeClassifierPlayer = m_menuPlayers->addAction(
        tr("Cascade Classifier Player"), this, SLOT(setCascadeClassifierPlayer()));
    playersGrp->addAction(actSetCascadeClassifierPlayer);
    actSetCascadeClassifierPlayer->setCheckable(true);
    actSetCascadeClassifierPlayer->setChecked(
        m_navigator->player()->type() == AbstractNavigationPlayer::CascadeClassifierType);

    m_menuPlayers->addSeparator();

    m_actPlayerConf = m_menuPlayers->addAction(
        tr("Player Configuration..."), this, SLOT(openPlayerConfDialog()));

    connect(m_menuPlayers, SIGNAL(aboutToShow()),
            this, SLOT(checkPlayerConfigurable()));


    m_menuTools = m_menuBar->addMenu(tr("Tools"));

    m_menuTools->addAction(tr("Plugin Logs"), this,
                           SLOT(showPluginLogsDialog()));
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

void MainMenuBarManager::setFixedRegionPlayer()
{
    m_navigator->setPlayer(
        new FixedRegionNavigationPlayer(m_navigator, m_imageView));
}

void MainMenuBarManager::setCascadeClassifierPlayer()
{
    m_navigator->setPlayer(
        new CascadeClassifierNavigationPlayer(m_navigator, m_imageView));
}

void MainMenuBarManager::checkPlayerConfigurable()
{
    m_actPlayerConf->setEnabled(m_navigator->player()->isConfigurable());
}

void MainMenuBarManager::openPlayerConfDialog()
{
    QDialog *confDialog = m_navigator->player()->configureDialog();
    confDialog->exec();
}

void MainMenuBarManager::showPluginLogsDialog()
{
    m_pluginLogsDialog->show();
}
