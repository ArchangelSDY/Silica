#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->graphicsView->setScene(&m_imageScene);

    QList<QUrl> imageUrls;
    imageUrls.append(QUrl("file:///home/sdy/Downloads/big.png"));
    imageUrls.append(QUrl("file:///home/sdy/Downloads/big.3.png"));

    connect(&m_navigator, SIGNAL(paint(QImage)), this, SLOT(paint(QImage)));
    m_navigator.setPlaylist(imageUrls);
    m_navigator.goIndex(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paint(QImage image)
{
    m_imageScene.addPixmap(QPixmap::fromImage(image));
}
