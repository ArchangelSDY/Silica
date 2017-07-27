#ifndef MAINMENUBARMANAGER_H
#define MAINMENUBARMANAGER_H

#include <QMenuBar>
#include <QObject>

class QDialog;
class ImagesCache;
class Navigator;
class NavigatorSynchronizer;

class MainMenuBarManager : public QObject
{
    Q_OBJECT
public:
    struct Context {
        Navigator *navigator;
        NavigatorSynchronizer *navigatorSynchronizer;
        QSharedPointer<ImagesCache> imagesCache;
        QMenuBar *menuBar;
        QWidget *imageView;
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
    Navigator *m_navigator;
    NavigatorSynchronizer *m_navigatorSynchronizer;
    QSharedPointer<ImagesCache> m_imagesCache;
    QWidget *m_imageView;

    QDialog *m_pluginLogsDialog;
};

#endif // MAINMENUBARMANAGER_H
