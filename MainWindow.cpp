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
#include "ui/GalleryItem.h"

#include "ui_MainWindow.h"

static const char* PLAYLIST_TITLE_PREFIX = "PlayList";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) ,
    m_database(new AsunaDatabase()) ,
    m_commandInterpreter(&m_navigator, m_database) ,
    m_inputMode(InputMode_Control)
{
    ui->setupUi(this);
    ui->centralWidget->layout()->setContentsMargins(0, 0, 0, 0);
    ui->centralWidget->layout()->setSpacing(0);
    ui->sidebar->hide();
    statusBar()->hide();

    // Window background
    QPalette newPalette(palette());
    newPalette.setColor(QPalette::Background, Qt::gray);
    setPalette(newPalette);

    // Move to screen center
    const QRect screen = QApplication::desktop()->screenGeometry();
    move(screen.center() - rect().center());

    ui->graphicsView->setScene(&m_imageScene);
    m_imageScene.setBackgroundBrush(Qt::gray);

    connect(&m_navigator, SIGNAL(paint(Image *)), this, SLOT(paint(Image *)));
    connect(&m_navigator, SIGNAL(paintThumbnail(Image*)),
            this, SLOT(paintThumbnail(Image*)));

    // StatusBar
    connect(statusBar(), SIGNAL(messageChanged(const QString &)),
            this, SLOT(statusBarMessageChanged(const QString &)));

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

    // Navigation binding for playListWidget
    connect(ui->playListWidget, SIGNAL(currentRowChanged(int)),
            &m_navigator, SLOT(goIndex(int)));

    // Navigation binding for galery
    connect(ui->gallery->scene(), SIGNAL(selectionChanged()),
            this, SLOT(gallerySelectionChanged()));

    connect(m_database, SIGNAL(gotPlayList(PlayList)),
            &m_navigator, SLOT(appendPlayList(PlayList)));
    connect(&m_commandInterpreter, SIGNAL(commandChange(QString)),
            this, SLOT(updateStatus(QString)));

    initUIStateMachines();

    processCommandLineOptions();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_database;
}

void MainWindow::initUIStateMachines()
{
    // Define states
    QState *galleryOnly = new QState();
    galleryOnly->assignProperty(ui->gallery, "visible", true);
    galleryOnly->assignProperty(ui->gallery, "maximumWidth", QWIDGETSIZE_MAX);
    galleryOnly->assignProperty(ui->graphicsView, "visible", false);
    m_exploreStateMachine.addState(galleryOnly);

    QState *viewOnly = new QState();
    viewOnly->assignProperty(ui->gallery, "visible", false);
    viewOnly->assignProperty(ui->graphicsView, "visible", true);
    viewOnly->assignProperty(ui->graphicsView, "maximumWidth", QWIDGETSIZE_MAX);
    m_exploreStateMachine.addState(viewOnly);

    QState *galleryAndView = new QState();
    galleryAndView->assignProperty(ui->gallery, "visible", true);
    galleryAndView->assignProperty(ui->graphicsView, "visible", true);
    connect(galleryAndView, SIGNAL(entered()),
        this, SLOT(layoutForGalleryAndView()));

    m_exploreStateMachine.addState(galleryAndView);

    // Define transitions
    viewOnly->addTransition(
        this, SIGNAL(transitToGalleryOnly()), galleryOnly);
    galleryAndView->addTransition(
        this, SIGNAL(transitToGalleryOnly()), galleryOnly);

    galleryOnly->addTransition(
        this, SIGNAL(transitToViewOnly()), viewOnly);
    galleryAndView->addTransition(
        this, SIGNAL(transitToViewOnly()), viewOnly);

    viewOnly->addTransition(
        this, SIGNAL(transitToGalleryAndView()), galleryAndView);
    galleryOnly->addTransition(
        this, SIGNAL(transitToGalleryAndView()), galleryAndView);

    galleryOnly->addTransition(
        ui->gallery, SIGNAL(transitToView()), viewOnly);
    galleryAndView->addTransition(
        ui->gallery, SIGNAL(transitToView()), viewOnly);

    m_exploreStateMachine.setInitialState(galleryAndView);
    m_exploreStateMachine.start();
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

    if (images.count() == 0) {
        return;
    }

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
    if (ui->gallery->scene()->selectedItems().count() > 0) {
        QString destDir = QFileDialog::getExistingDirectory(
            this, "Save to", GlobalConfig::instance()->wallpaperDir());

        if (destDir.isEmpty()) {
            return;
        }

        foreach (QGraphicsItem *item, ui->gallery->scene()->selectedItems()) {
            GalleryItem *galleryItem = static_cast<GalleryItem *>(item);
            Image *image = galleryItem->image();
            if (image) {
                QString destPath = destDir + QDir::separator() + image->name();
                bool success = image->copy(destPath);
                QString msg;
                if (success) {
                    QTextStream(&msg) << image->name() << " saved!";
                } else {
                    QTextStream(&msg) << "Error occured when saving "
                                      << image->name();
                }
                statusBar()->showMessage(msg, 2000);
            }
        }
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
    // PlayList widget
    ui->playListWidget->setCurrentRow(index);

    // Gallery widget
    QGraphicsItem *selectedItem =
        ui->gallery->scene()->items(Qt::AscendingOrder).at(index);
    if (selectedItem) {
        ui->gallery->scene()->clearSelection();
        selectedItem->setSelected(true);
        selectedItem->ensureVisible();
    }

    // Sidebar
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
    } else {
        statusBar()->clearMessage();
    }
}

void MainWindow::gallerySelectionChanged()
{
    QGraphicsScene *scene = ui->gallery->scene();
    if (scene->selectedItems().length() > 0) {
        QGraphicsItem * const item = scene->selectedItems()[0];
        const QList<QGraphicsItem *> &allItems =
            scene->items(Qt::AscendingOrder);
        int index = allItems.indexOf(item);
        m_navigator.goIndex(index);
    }
}

void MainWindow::keyPressEvent(QKeyEvent *ev)
{
    if (m_inputMode == InputMode_Control) {
        handleControlKeyPress(ev);
    } else if (m_inputMode == InputMode_Command) {
        handleCommandKeyPress(ev);

        if (m_commandInterpreter.isEmpty()) {
            m_inputMode = InputMode_Control;
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
        case Qt::Key_G:
            emit transitToGalleryOnly();
            break;
        case Qt::Key_V:
            emit transitToViewOnly();
            break;
        case Qt::Key_B:
            emit transitToGalleryAndView();
            break;
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
            m_inputMode = InputMode_Command;
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
        m_inputMode = InputMode_Control;
        m_commandInterpreter.reset();
    } else {
        m_commandInterpreter.keyPress(ev);
    }
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    ui->graphicsView->fitInViewIfNecessary();
    layoutForGalleryAndView();
}

void MainWindow::layoutForGalleryAndView()
{
    int panelsCount = ui->centralWidget->layout()->count();
    if (panelsCount > 0) {
        int panelWidth = width() / panelsCount;
        for (int i = 0; i < panelsCount; ++i) {
            QLayoutItem *panel = ui->centralWidget->layout()->itemAt(i);
            panel->setGeometry(QRect(
                (panelsCount - i - 1) * panelWidth, 0, panelWidth, height()));
        }
    }
}

void MainWindow::statusBarMessageChanged(const QString & message)
{
    if (message.isEmpty()) {
        statusBar()->hide();
    } else {
        statusBar()->show();
    }
}
