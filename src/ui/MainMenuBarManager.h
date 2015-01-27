#ifndef MAINMENUBARMANAGER_H
#define MAINMENUBARMANAGER_H

#include <QMenuBar>
#include <QObject>

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

private slots:
    void setNormalPlayer();
    void setHotspotsPlayer();
    void setExpandingPlayer();
    void setFixedRegionPlayer();

private:
    void init();


    QMenuBar *m_menuBar;
    Navigator *m_navigator;
    QWidget *m_imageView;
};

#endif // MAINMENUBARMANAGER_H
