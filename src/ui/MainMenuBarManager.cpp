#include "MainMenuBarManager.h"

#include <QDialog>
#include <QScopedPointer>

#include "navigation/NavigationPlayerManager.h"
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

    QList<AbstractNavigationPlayer *> players = NavigationPlayerManager::instance()->all();
    Navigator *navigator = m_navigator;
    for (AbstractNavigationPlayer *player : players) {
        QAction *actPlayer = m_menuPlayers->addAction(player->name(), [navigator, player]() {
            navigator->setPlayer(player);
        });
        actPlayer->setCheckable(true);
        actPlayer->setChecked(navigator->player() == player);
        playersGrp->addAction(actPlayer);
    }

    m_menuPlayers->addSeparator();

    m_actPlayerConf = m_menuPlayers->addAction(
        tr("Player Configuration..."), this, SLOT(openPlayerConfDialog()));

    connect(m_menuPlayers, SIGNAL(aboutToShow()),
            this, SLOT(checkPlayerConfigurable()));


    m_menuTools = m_menuBar->addMenu(tr("Tools"));

    m_menuTools->addAction(tr("Plugin Logs"), this,
                           SLOT(showPluginLogsDialog()));
}

void MainMenuBarManager::checkPlayerConfigurable()
{
    m_actPlayerConf->setEnabled(m_navigator->player()->isConfigurable());
}

void MainMenuBarManager::openPlayerConfDialog()
{
    QScopedPointer<QDialog> confDialog(m_navigator->player()->createConfigureDialog());
    confDialog->exec();
}

void MainMenuBarManager::showPluginLogsDialog()
{
    m_pluginLogsDialog->show();
}
