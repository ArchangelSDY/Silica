#include <QDesktopWidget>
#include <QDockWidget>
#include <QFileDialog>
#include <QGraphicsPixmapItem>
#include <QMessageBox>
#include <QStatusBar>

#include "db/AsunaDatabase.h"
#include "PlayList.h"
#include "MainWindow.h"
#include "ui_MainWindow.h"

static const char* PLAYLIST_TITLE_PREFIX = "PlayList";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) ,
    m_database(new AsunaDatabase()) ,
    m_fitInWindow(true) ,
    m_inputMode(ControlMode)
{
    ui->setupUi(this);
    ui->centralWidget->layout()->setContentsMargins(0, 0, 0, 0);
    ui->sidebar->hide();
    statusBar()->setVisible(false);

    // Move to screen center
    const QRect screen = QApplication::desktop()->screenGeometry();
    move(screen.center() - rect().center());

    ui->graphicsView->setScene(&m_imageScene);
    m_imageScene.setBackgroundBrush(Qt::gray);

    connect(&m_navigator, SIGNAL(paint(Image *)), this, SLOT(paint(Image *)));
    connect(&m_navigator, SIGNAL(paintThumbnail(Image*)),
            this, SLOT(paintThumbnail(Image*)));
    connect(&m_navigator, SIGNAL(playListChange(PlayList *)),
            this, SLOT(playListChange(PlayList *)));
    connect(&m_navigator, SIGNAL(navigationChange(int)),
            this, SLOT(navigationChange(int)));
    connect(ui->playListWidget, SIGNAL(currentRowChanged(int)),
            &m_navigator, SLOT(goIndex(int)));
    connect(m_database, SIGNAL(gotPlayList(PlayList)),
            &m_navigator, SLOT(setPlayList(PlayList)));

    processCommandLineOptions();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_database;
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

void MainWindow::paint(Image *image)
{
    if (image) {
        QPixmap pixmap = QPixmap::fromImage(image->data());
        m_imageScene.clear();
        m_imageScene.setSceneRect(pixmap.rect());
        QGraphicsPixmapItem *item = m_imageScene.addPixmap(pixmap);
        item->setTransformationMode(Qt::SmoothTransformation);
        fitInWindowIfNecessary();

        QString status = (image->status() ==
                          Image::LoadComplete) ? "" : "[Loading]";
        QString title;
        QTextStream(&title) << status << image->name();

        setWindowTitle(title);
    }
}

void MainWindow::paintThumbnail(Image *image)
{
    if (image) {
        const QSize &viewSize = ui->graphicsView->size();
        QPixmap rawThumbnail = QPixmap::fromImage(image->thumbnail());
        QPixmap fitThumbnail = rawThumbnail.scaled(
            viewSize, Qt::KeepAspectRatioByExpanding);

        m_imageScene.clear();
        m_imageScene.setSceneRect(fitThumbnail.rect());

        QGraphicsPixmapItem *item = m_imageScene.addPixmap(fitThumbnail);
        item->setTransformationMode(Qt::SmoothTransformation);

        ui->graphicsView->fitInView(fitThumbnail.rect(), Qt::KeepAspectRatio);
    }
}

void MainWindow::playListChange(PlayList *playList)
{
    QListWidget *list = ui->playListWidget;
    list->clear();
    foreach (const QUrl &url, *playList) {
        if (url.scheme() != "zip") {
            list->addItem(QFileInfo(url.toLocalFile()).completeBaseName());
        } else {
            list->addItem(url.fragment());
        }
    }
}

void MainWindow::navigationChange(int index)
{
    ui->playListWidget->setCurrentRow(index);
    QString title;
    QTextStream(&title) << PLAYLIST_TITLE_PREFIX << " - "
        << (index + 1) << "/" << m_navigator.playList().count();
    ui->sidebar->setWindowTitle(title);
}

void MainWindow::keyPressEvent(QKeyEvent *ev)
{
    if (m_inputMode == ControlMode) {
        handleControlKeyPress(ev);
    } else if (m_inputMode == CommandMode) {
        handleCommandKeyPress(ev);
    }
}

void MainWindow::handleControlKeyPress(QKeyEvent *ev)
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
            if (QMessageBox::question(this, "Exit", "Exit?") ==
                    QMessageBox::Yes) {
                QApplication::exit();
            }
            break;
        case Qt::Key_Slash:
            m_inputMode = CommandMode;
            statusBar()->showMessage("Command Mode");
            statusBar()->show();
            break;
        case Qt::Key_T: {
            QDockWidget *sidebar = ui->sidebar;
            sidebar->setVisible(!sidebar->isVisible());
            break;
        }
        case Qt::Key_A:
            m_database->queryByTag("pantsu");
            break;
    }
}

void MainWindow::handleCommandKeyPress(QKeyEvent *ev)
{
    switch (ev->key()) {
        case Qt::Key_Escape:
            m_inputMode = ControlMode;
            statusBar()->clearMessage();
            statusBar()->hide();
            break;
    }

}

void MainWindow::resizeEvent(QResizeEvent *)
{
    fitInWindowIfNecessary();
}
