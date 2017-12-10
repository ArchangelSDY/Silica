#ifndef MAINMENUBARMANAGER_H
#define MAINMENUBARMANAGER_H

#include <QMenuBar>
#include <QObject>

class QDialog;
class QHBoxLayout;
class ImagesCache;
class Navigator;
class NavigatorSynchronizer;

class MainMenuBarManager : public QObject
{
    Q_OBJECT
public:
    struct Context {
        Navigator *primaryNavigator;
        Navigator *secondaryNavigator;
        QWidget **pPrimaryGraphicsView;
        QWidget **pSecondaryGraphicsView;
        NavigatorSynchronizer *navigatorSynchronizer;
        QSharedPointer<ImagesCache> imagesCache;
        QMenuBar *menuBar;
        QHBoxLayout *imageViewsParentLayout;
    };

    explicit MainMenuBarManager(Context context, QObject *parent = 0);
    ~MainMenuBarManager();

private slots:
    void openPlayerConfDialog();
    void showPluginLogsDialog();

private:
    void init();
    void createMenuNavigationPlayers(QMenu *parentMenu);
    void createMenuNavigationTwoColumns(QMenu *parentMenu);
    void createMenuNavigationLoop(QMenu *parentMenu);
    void createMenuNavigationAutoSpeed(QMenu *parentMenu);

    QMenuBar *m_menuBar;
    Navigator *m_primaryNavigator;
    Navigator *m_secondaryNavigator;
    QWidget **m_pPrimaryGraphicsView;
    QWidget **m_pSecondaryGraphicsView;
    NavigatorSynchronizer *m_navigatorSynchronizer;
    QSharedPointer<ImagesCache> m_imagesCache;
    QHBoxLayout *m_imageViewsParentLayout;

    QDialog *m_pluginLogsDialog;
};

#endif // MAINMENUBARMANAGER_H
