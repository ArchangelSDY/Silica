#include <QDesktopWidget>
#include <QDockWidget>
#include <QFileDialog>
#include <QGraphicsPixmapItem>
#include <QMessageBox>
#include <QStatusBar>

#include "db/AsunaDatabase.h"
#include "GlobalConfig.h"
#include "PlayList.h"
#include "MainWindow.h"
#include "ui_MainWindow.h"

static const char* PLAYLIST_TITLE_PREFIX = "PlayList";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) ,
    m_database(new AsunaDatabase()) ,
    m_commandInterpreter(&m_navigator, m_database) ,
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

    // Update sidebar
    connect(&m_navigator, SIGNAL(playListChange(PlayList)),
            this, SLOT(playListChange(PlayList)));
    connect(&m_navigator, SIGNAL(playListAppend(PlayList)),
            this, SLOT(playListAppend(PlayList)));

    // Update gallery
    connect(&m_navigator, SIGNAL(playListChange(PlayList)),
            ui->gallery, SLOT(playListChange(PlayList)));
    connect(&m_navigator, SIGNAL(playListAppend(PlayList)),
            ui->gallery, SLOT(playListAppend(PlayList)));

    connect(&m_navigator, SIGNAL(navigationChange(int)),
            this, SLOT(navigationChange(int)));
    connect(ui->playListWidget, SIGNAL(currentRowChanged(int)),
            &m_navigator, SLOT(goIndex(int)));
    connect(m_database, SIGNAL(gotPlayList(PlayList)),
            &m_navigator, SLOT(appendPlayList(PlayList)));
    connect(&m_commandInterpreter, SIGNAL(commandChange(QString)),
            this, SLOT(updateStatus(QString)));

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

void MainWindow::promptToOpen()
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

void MainWindow::promptToSave()
{
    if (m_navigator.currentImage()) {
        QString destDir = QFileDialog::getExistingDirectory(
            this, "Save to", GlobalConfig::instance()->wallpaperDir());
        QString fileName = m_navigator.currentImage()->name();
        QString destPath = destDir + QDir::separator() + fileName;

        m_navigator.currentImage()->copy(destPath);
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
        ui->graphicsView->fitInViewIfNecessary();


        QString status;
        if (image->status() == Image::Loading) {
            status = "[Loading]";
        } else if (image->status() == Image::LoadError) {
            status = "[Error]";
        } else {
            status = "";
        }

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

void MainWindow::playListChange(PlayList playList)
{
    ui->playListWidget->clear();

    playListAppend(playList);
}

void MainWindow::playListAppend(PlayList appended)
{
    QListWidget *list = ui->playListWidget;
    foreach (const QSharedPointer<Image> &image, appended) {
        list->addItem(image->name());
    }

    updateSidebarTitle();
}

void MainWindow::navigationChange(int index)
{
    ui->playListWidget->setCurrentRow(index);

    updateSidebarTitle();
}

void MainWindow::updateSidebarTitle()
{
    int index = m_navigator.currentIndex();
    QString title;
    QTextStream(&title) << PLAYLIST_TITLE_PREFIX << " - "
        << (index + 1) << "/" << m_navigator.playList().count();
    ui->sidebar->setWindowTitle(title);
}

void MainWindow::updateStatus(QString message)
{
    if (!message.isEmpty()) {
        statusBar()->showMessage(message);
        statusBar()->show();
    } else {
        statusBar()->clearMessage();
        statusBar()->hide();
    }
}

void MainWindow::keyPressEvent(QKeyEvent *ev)
{
    if (m_inputMode == ControlMode) {
        handleControlKeyPress(ev);
    } else if (m_inputMode == CommandMode) {
        handleCommandKeyPress(ev);

        if (m_commandInterpreter.isEmpty()) {
            m_inputMode = ControlMode;
        }
    }
}

void MainWindow::handleControlKeyPress(QKeyEvent *ev)
{
    switch (ev->key()) {
        case Qt::Key_J:
        case Qt::Key_Space:
            m_navigator.goNext();
            break;
        case Qt::Key_K:
            m_navigator.goPrev();
            break;
        case Qt::Key_F:
            ui->graphicsView->toggleFitInView();
            ui->graphicsView->fitInViewIfNecessary();
            break;
        case Qt::Key_O: {
            promptToOpen();
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
            handleCommandKeyPress(ev);
            break;
        case Qt::Key_S:
            promptToSave();
            break;
        case Qt::Key_T: {
            QDockWidget *sidebar = ui->sidebar;
            sidebar->setVisible(!sidebar->isVisible());
            break;
        }
        case Qt::Key_Home:
            m_navigator.goFirst();
            break;
        case Qt::Key_End:
            m_navigator.goLast();
            break;
        case Qt::Key_1:
        case Qt::Key_2:
        case Qt::Key_3:
        case Qt::Key_4:
        case Qt::Key_5:
        case Qt::Key_6:
        case Qt::Key_7:
        case Qt::Key_8:
        case Qt::Key_9:
            ui->graphicsView->fitGridInView(ev->text().toInt());
            break;
    }

    ev->accept();
}

void MainWindow::handleCommandKeyPress(QKeyEvent *ev)
{
    if (ev->key() == Qt::Key_Escape) {
        m_inputMode = ControlMode;
        m_commandInterpreter.reset();
    } else {
        m_commandInterpreter.keyPress(ev);
    }
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    ui->graphicsView->fitInViewIfNecessary();
}
