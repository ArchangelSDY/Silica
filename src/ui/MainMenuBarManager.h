#ifndef MAINMENUBARMANAGER_H
#define MAINMENUBARMANAGER_H

#include <QMenuBar>
#include <QObject>

class QDialog;
class Navigator;

class MainMenuBarManager : public QObject
{
    Q_OBJECT
public:
    struct Context {
        Navigator *navigator;
        QMenuBar *menuBar;
        QWidget *imageView;
    };

    explicit MainMenuBarManager(Context context, QObject *parent = 0);
    ~MainMenuBarManager();

private slots:
    void checkPlayerConfigurable();
    void openPlayerConfDialog();
    void showPluginLogsDialog();

private:
    void init();


    QMenuBar *m_menuBar;
    Navigator *m_navigator;
    QWidget *m_imageView;

    QMenu *m_menuPlayers;
    QAction *m_actPlayerConf;

    QMenu *m_menuTools;
    QDialog *m_pluginLogsDialog;
};

#endif // MAINMENUBARMANAGER_H
