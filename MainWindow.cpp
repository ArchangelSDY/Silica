#include <QGraphicsPixmapItem>

#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) ,
    m_fitInWindow(true)
{
    ui->setupUi(this);
    ui->centralWidget->layout()->setContentsMargins(0, 0, 0, 0);
    ui->graphicsView->setScene(&m_imageScene);
    m_imageScene.setBackgroundBrush(Qt::gray);

    connect(&m_navigator, SIGNAL(paint(QImage)), this, SLOT(paint(QImage)));

    QStringList args = QApplication::arguments();
    if (args.length() > 1) {
        QString dirPath = args.last();
        m_navigator.openDir(dirPath);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::fitInWindowIfNecessary()
{
    if (m_fitInWindow) {
        ui->graphicsView->fitInView(
            ui->graphicsView->sceneRect(), Qt::KeepAspectRatio);
    } else {
        ui->graphicsView->resetMatrix();
    }
}

void MainWindow::paint(QImage image)
{
    QPixmap pixmap = QPixmap::fromImage(image);
    m_imageScene.clear();
    m_imageScene.setSceneRect(pixmap.rect());
    QGraphicsPixmapItem *item = m_imageScene.addPixmap(pixmap);
    item->setTransformationMode(Qt::SmoothTransformation);
    fitInWindowIfNecessary();
}

void MainWindow::keyPressEvent(QKeyEvent *ev)
{
    switch (ev->key()) {
        case Qt::Key_J:
            m_navigator.goNext();
            break;
        case Qt::Key_K:
            m_navigator.goPrev();
            break;
        case Qt::Key_F:
            m_fitInWindow = !m_fitInWindow;
            fitInWindowIfNecessary();
            break;
    }
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    fitInWindowIfNecessary();
}
