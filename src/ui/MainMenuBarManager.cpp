#include "MainMenuBarManager.h"

#include <QDialog>
#include <QHBoxLayout>
#include <QScopedPointer>

#include "image/caches/ImagesCache.h"
#include "image/caches/LoopImagesCacheStrategy.h"
#include "image/caches/NormalImagesCacheStrategy.h"
#include "navigation/NavigationPlayerManager.h"
#include "navigation/NavigatorSynchronizer.h"
#include "ui/PluginLogsDialog.h"
#include "Navigator.h"

MainMenuBarManager::MainMenuBarManager(Context context, QObject *parent) :
    QObject(parent) ,
    m_menuBar(context.menuBar) ,
    m_navigator(context.navigator) ,
    m_navigatorSynchronizer(context.navigatorSynchronizer) ,
    m_imagesCache(context.imagesCache) ,
    m_pluginLogsDialog(new PluginLogsDialog()) ,
    m_imageViewsParentLayout(context.imageViewsParentLayout)
{
    init();
}

MainMenuBarManager::~MainMenuBarManager()
{
    delete m_pluginLogsDialog;
}

void MainMenuBarManager::init()
{
    // Navigation
    QMenu *menuNavigation = m_menuBar->addMenu(tr("Navigation"));
    createMenuNavigationPlayers(menuNavigation);
    createMenuNavigationTwoColumns(menuNavigation);
    createMenuNavigationLoop(menuNavigation);
    createMenuNavigationAutoSpeed(menuNavigation);

    // Tools
    QMenu *menuTools = m_menuBar->addMenu(tr("Tools"));
    menuTools->addAction(tr("Plugin Logs"), this,
                         SLOT(showPluginLogsDialog()));
}

void MainMenuBarManager::createMenuNavigationPlayers(QMenu *parentMenu)
{
    QMenu *menuPlayers = parentMenu->addMenu(tr("Players"));

    QActionGroup *playersGrp = new QActionGroup(menuPlayers);
    playersGrp->setExclusive(true);

    QList<AbstractNavigationPlayer *> players = NavigationPlayerManager::instance()->all();
    Navigator *navigator = m_navigator;
    for (AbstractNavigationPlayer *player : players) {
        QAction *actPlayer = menuPlayers->addAction(player->name(), [navigator, player]() {
            navigator->setPlayer(player);
        });
        actPlayer->setCheckable(true);
        actPlayer->setChecked(navigator->player() == player);
        playersGrp->addAction(actPlayer);
    }

    menuPlayers->addSeparator();

    QAction *actPlayerConf = menuPlayers->addAction(
        tr("Player Configuration..."), this, SLOT(openPlayerConfDialog()));

    connect(menuPlayers, &QMenu::aboutToShow, [actPlayerConf, navigator]() {
        actPlayerConf->setEnabled(navigator->player()->isConfigurable());
    });
}

void MainMenuBarManager::createMenuNavigationTwoColumns(QMenu *parentMenu)
{
    QMenu *menuTwoColumns = parentMenu->addMenu(tr("Two Columns"));

    QHBoxLayout *columnsLayout = m_imageViewsParentLayout;
    QAction *actLTR = menuTwoColumns->addAction(tr("Left to Right"));
    actLTR->setCheckable(true);
    QAction *actRTL = menuTwoColumns->addAction(tr("Right to Left"));
    actRTL->setCheckable(true);
    actRTL->setChecked(true);

    // One slot is enough as toggled() signal will be emitted twice for both actions
    connect(actLTR, &QAction::toggled, [columnsLayout]() {
        if (columnsLayout->count() == 1) {
            return;
        }

        // Swap
        QLayoutItem *first = columnsLayout->takeAt(0);
        columnsLayout->addItem(first);
    });

    QActionGroup *grp = new QActionGroup(menuTwoColumns);
    grp->addAction(actLTR);
    grp->addAction(actRTL);
}

void MainMenuBarManager::createMenuNavigationLoop(QMenu *parentMenu)
{
    Navigator *primaryNavigator = m_navigator;
    NavigatorSynchronizer *navigatorSynchronizer = m_navigatorSynchronizer;
    QSharedPointer<ImagesCache> imagesCache = m_imagesCache;
    QAction *loop = parentMenu->addAction(tr("Loop"), [primaryNavigator, navigatorSynchronizer, imagesCache](bool loop) {
        navigatorSynchronizer->setLoop(loop);

        if (loop) {
            imagesCache->setStrategy(
                new LoopImagesCacheStrategy(imagesCache.data(), primaryNavigator));
        } else {
            imagesCache->setStrategy(
                new NormalImagesCacheStrategy(imagesCache.data()));
        }
    });
    loop->setCheckable(true);
    connect(parentMenu, &QMenu::aboutToShow, [this, loop]() {
        loop->setChecked(this->m_navigator->isLooping());
    });
}

void MainMenuBarManager::createMenuNavigationAutoSpeed(QMenu *parentMenu)
{
    int currentNavInterval = m_navigator->autoNavigationInterval();
    QMenu *autoNavMenu = parentMenu->addMenu(tr("Auto Speed"));
    QActionGroup *autoNavGrp = new QActionGroup(autoNavMenu);
    QSignalMapper *autoNavSigMap = new QSignalMapper(autoNavMenu);
    connect(autoNavSigMap, SIGNAL(mapped(int)),
            m_navigator, SLOT(setAutoNavigationInterval(int)));

    QAction *fastAutoNav = autoNavMenu->addAction(
        tr("Fast"), autoNavSigMap, SLOT(map()));
    fastAutoNav->setCheckable(true);
    fastAutoNav->setChecked(
        currentNavInterval == Navigator::FAST_AUTO_NAVIGATION_INTERVAL);
    autoNavGrp->addAction(fastAutoNav);
    autoNavSigMap->setMapping(
        fastAutoNav, Navigator::FAST_AUTO_NAVIGATION_INTERVAL);

    QAction *mediumAutoNav = autoNavMenu->addAction(
        tr("Medium"), autoNavSigMap, SLOT(map()));
    mediumAutoNav->setCheckable(true);
    mediumAutoNav->setChecked(
        currentNavInterval == Navigator::MEDIUM_AUTO_NAVIGATION_INTERVAL);
    autoNavGrp->addAction(mediumAutoNav);
    autoNavSigMap->setMapping(
        mediumAutoNav, Navigator::MEDIUM_AUTO_NAVIGATION_INTERVAL);

    QAction *slowAutoNavi = autoNavMenu->addAction(
        tr("Slow"), autoNavSigMap, SLOT(map()));
    slowAutoNavi->setCheckable(true);
    slowAutoNavi->setChecked(
        currentNavInterval == Navigator::SLOW_AUTO_NAVIGATION_INTERVAL);
    autoNavGrp->addAction(slowAutoNavi);
    autoNavSigMap->setMapping(
        slowAutoNavi, Navigator::SLOW_AUTO_NAVIGATION_INTERVAL);
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
