#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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

signals:
    void transitToGalleryOnly();
    void transitToGalleryAndView();
    void transitToViewOnly();

public slots:
    void playListChange(PlayList playList);
    void playListAppend(PlayList appended);
    void navigationChange(int index);
    void updateStatus(QString message);
    void gallerySelectionChanged();
    void layoutForGalleryAndView();
    void statusBarMessageChanged(const QString & message);

private slots:
    void imageLoaded(Image *image);

protected:
    virtual void keyPressEvent(QKeyEvent *);
    virtual void resizeEvent(QResizeEvent *);

private:
    void initUIStateMachines();
    void processCommandLineOptions();
    void promptToOpen();
    void promptToSave();

    void handleControlKeyPress(QKeyEvent *);
    void handleCommandKeyPress(QKeyEvent *);

    void updateSidebarTitle();

    enum InputMode {
        InputMode_Control,
        InputMode_Command,
    };

    Ui::MainWindow *ui;

    Navigator m_navigator;
    RemoteDatabase *m_database;
    CommandInterpreter m_commandInterpreter;
    InputMode m_inputMode;
    QStateMachine m_exploreStateMachine;
};

#endif // MAINWINDOW_H
