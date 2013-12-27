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
    void paint(QImage image);

protected:
    virtual void keyPressEvent(QKeyEvent *);

private:
    Ui::MainWindow *ui;

    Navigator m_navigator;
    QGraphicsScene m_imageScene;
    bool m_fitInWindow;
};

#endif // MAINWINDOW_H
