#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>

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

public slots:
    void paint(Image *image);
    void paintThumbnail(Image *image);
    void playListChange(PlayList *playList);
    void playListAppend(PlayList *appended);
    void navigationChange(int index);

protected:
    virtual void keyPressEvent(QKeyEvent *);
    virtual void resizeEvent(QResizeEvent *);

private:
    void processCommandLineOptions();
    void promptToChooseFiles();

    void fitInWindowIfNecessary();

    void handleControlKeyPress(QKeyEvent *);
    void handleCommandKeyPress(QKeyEvent *);

    enum Mode {
        ControlMode,
        CommandMode,
    };

    Ui::MainWindow *ui;

    Navigator m_navigator;
    Database *m_database;
    QGraphicsScene m_imageScene;
    bool m_fitInWindow;
    Mode m_inputMode;
};

#endif // MAINWINDOW_H
