#include <QDesktopWidget>
#include <QFileDialog>
#include <QGraphicsPixmapItem>

#include "PlayList.h"
#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) ,
    m_fitInWindow(true)
{
    ui->setupUi(this);
    ui->centralWidget->layout()->setContentsMargins(0, 0, 0, 0);

    // Move to screen center
    const QRect screen = QApplication::desktop()->screenGeometry();
    move(screen.center() - rect().center());

    ui->graphicsView->setScene(&m_imageScene);
    m_imageScene.setBackgroundBrush(Qt::gray);

    connect(&m_navigator, SIGNAL(paint(QImage)), this, SLOT(paint(QImage)));

    processCommandLineOptions();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::processCommandLineOptions()
{
    QCommandLineParser parser;
    parser.addPositionalArgument("paths", "Directories or files", "[paths...]");

    parser.process(*QCoreApplication::instance());
    const QStringList imagePaths = parser.positionalArguments();

    PlayList playList;
    foreach (const QString& imagePath, imagePaths) {
        if (imagePath.startsWith("file://") || imagePath.startsWith("zip://")) {
            playList.addPath(QUrl(imagePath));
        } else {
            playList.addPath(imagePath);
        }
    }

    m_navigator.setPlayList(playList);
}

void MainWindow::promptToChooseFiles()
{
    QList<QUrl> images = QFileDialog::getOpenFileUrls(
        this, QString(), QUrl(),
        "All (*.png *.jpg *.zip);;Images (*.png *.jpg);;Zip (*.zip)");

    // Hack for zip
    // FIXME: Better way to handle this?
    for (QList<QUrl>::iterator i = images.begin(); i != images.end(); ++i) {
        if ((*i).path().endsWith(".zip")) {
            (*i).setScheme("zip");
        }
    }

    PlayList playList(images);
    m_navigator.setPlayList(playList);
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
        case Qt::Key_O: {
            promptToChooseFiles();
            break;
        }
        case Qt::Key_F11:
            if (!isFullScreen()) {
                setWindowState(Qt::WindowFullScreen);
            } else {
                setWindowState(Qt::WindowNoState);
            }
            break;
        case Qt::Key_Escape:
            QApplication::exit();
            break;
    }
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    fitInWindowIfNecessary();
}
