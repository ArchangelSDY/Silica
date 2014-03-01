#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QActionGroup>
#include <QMainWindow>
#include <QGraphicsScene>

#include "CommandInterpreter.h"
#include "RemoteDatabase.h"
#include "Navigator.h"
#include "PlayList.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void playListChange(PlayList playList);
    void playListAppend(PlayList appended);
    void navigationChange(int index);
    void updateStatus(QString message);
    void gallerySelectionChanged();
    void statusBarMessageChanged(const QString & message);

private slots:
    void imageLoaded(Image *image);
    void loadSavedPlayLists();
    void loadSelectedPlayList();

protected:
    virtual void keyPressEvent(QKeyEvent *);
    virtual void resizeEvent(QResizeEvent *);
    virtual void changeEvent(QEvent *);

private:
    void setupExtraUi();
    void processCommandLineOptions();
    void promptToOpenImage();
    void promptToSaveImage();
    void promptToSavePlayList();

    void handleControlKeyPress(QKeyEvent *);
    void handleCommandKeyPress(QKeyEvent *);

    void updateSidebarTitle();
    void switchViews();

    enum InputMode {
        InputMode_Control,
        InputMode_Command,
    };

    Ui::MainWindow *ui;
    QToolBar *m_toolBar;
    QActionGroup *m_toolBarActs;
    QAction *m_actToolBarFav;
    QAction *m_actToolBarGallery;
    QAction *m_actToolBarImage;

    Navigator m_navigator;
    RemoteDatabase *m_database;
    CommandInterpreter m_commandInterpreter;
    InputMode m_inputMode;
};

#endif // MAINWINDOW_H
