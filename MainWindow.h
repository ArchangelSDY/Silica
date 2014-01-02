#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>

#include "Navigator.h"

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

protected:
    virtual void keyPressEvent(QKeyEvent *);
    virtual void resizeEvent(QResizeEvent *);

private:
    void processCommandLineOptions();
    void promptToChooseFiles();

    void fitInWindowIfNecessary();

    Ui::MainWindow *ui;

    Navigator m_navigator;
    QGraphicsScene m_imageScene;
    bool m_fitInWindow;
};

#endif // MAINWINDOW_H
