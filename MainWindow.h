#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>

#include "CommandInterpreter.h"
#include "db/Database.h"
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
    void paint(Image *image);
    void paintThumbnail(Image *image);
    void playListChange(PlayList playList);
    void playListAppend(PlayList appended);
    void navigationChange(int index);
    void updateStatus(QString message);
    void gallerySelectionChanged();
    void layoutForGalleryAndView();

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
    Database *m_database;
    CommandInterpreter m_commandInterpreter;
    QGraphicsScene m_imageScene;
    InputMode m_inputMode;
    QStateMachine m_exploreStateMachine;
};

#endif // MAINWINDOW_H
