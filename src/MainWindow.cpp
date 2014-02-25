#include <QDesktopWidget>
#include <QDockWidget>
#include <QFileDialog>
#include <QGraphicsPixmapItem>
#include <QInputDialog>
#include <QMessageBox>
#include <QStatusBar>
#include <QToolBar>

#include "AsunaDatabase.h"
#include "GlobalConfig.h"
#include "ImageGalleryItem.h"
#include "ImageSourceManager.h"
#include "PlayList.h"
#include "PlayListRecord.h"
#include "MainWindow.h"
#include "AbstractGalleryItem.h"

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
    setupExtraUi();

    ui->gallery->setNavigator(&m_navigator);
    ui->sidebar->hide();
    statusBar()->hide();

    // Move to screen center
    const QRect screen = QApplication::desktop()->screenGeometry();
    move(screen.center() - rect().center());

    connect(&m_navigator, SIGNAL(paint(Image *)),
            this, SLOT(imageLoaded(Image *)));
    connect(&m_navigator, SIGNAL(paint(Image *)),
            ui->graphicsView, SLOT(paint(Image *)));
    connect(&m_navigator, SIGNAL(paintThumbnail(Image *)),
            ui->graphicsView, SLOT(paintThumbnail(Image *)));

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

    processCommandLineOptions();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_database;
}

void MainWindow::setupExtraUi()
{
    // Main window
    ui->centralWidget->layout()->setContentsMargins(0, 0, 0, 0);
    ui->centralWidget->layout()->setSpacing(0);
    setStyleSheet(
        "QGraphicsView {"
            "border: none; "
        "}"
    );


    // Main toolbar
    QToolBar *toolBar = new QToolBar(this);
    toolBar->setFloatable(false);
    toolBar->setMovable(false);
    toolBar->setOrientation(Qt::Vertical);
    toolBar->setIconSize(QSize(24, 24));
    toolBar->setStyleSheet(
        "* {"
            "color: #FFFFFF;"
            "background-color: #3C414C;"
            "border: none;"
            "padding: 5px;"
        "}"
        "QToolBar {"
            "border-right: 1px solid black;"
        "}"
    );

    QActionGroup *toolBarActGrp = new QActionGroup(this);
    QSignalMapper *toolBarSigMapper = new QSignalMapper(this);
    connect(toolBarSigMapper, SIGNAL(mapped(int)),
            ui->stackedViews, SLOT(setCurrentIndex(int)));

    // Fav icon
    QIcon toolBarFavIcon(":/toolbar/fav.png");
    toolBarFavIcon.addFile(":/toolbar/fav-active.png",
                           QSize(), QIcon::Active, QIcon::On);
    QAction *actToolBarFav = toolBar->addAction(
        toolBarFavIcon, tr("Favourite"), toolBarSigMapper, SLOT(map()));
    actToolBarFav->setCheckable(true);
    toolBarActGrp->addAction(actToolBarFav);
    toolBarSigMapper->setMapping(actToolBarFav, 0);

    // Gallery icon
    QIcon toolBarGalleryIcon(":/toolbar/gallery.png");
    toolBarGalleryIcon.addFile(":/toolbar/gallery-active.png",
                               QSize(), QIcon::Active, QIcon::On);
    QAction *actToolBarGallery = toolBar->addAction(
        toolBarGalleryIcon, tr("Gallery"), toolBarSigMapper, SLOT(map()));
    actToolBarGallery->setCheckable(true);
    toolBarActGrp->addAction(actToolBarGallery);
    toolBarSigMapper->setMapping(actToolBarGallery, 1);

    // Image icon
    QIcon toolBarImageIcon(":/toolbar/image-view.png");
    toolBarImageIcon.addFile(":/toolbar/image-view-active.png",
                             QSize(), QIcon::Active, QIcon::On);
    QAction *actToolBarImage = toolBar->addAction(
        toolBarImageIcon, tr("Image"), toolBarSigMapper, SLOT(map()));
    actToolBarImage->setCheckable(true);
    toolBarActGrp->addAction(actToolBarImage);
    toolBarSigMapper->setMapping(actToolBarImage, 2);

    actToolBarFav->setChecked(true);
    addToolBar(Qt::LeftToolBarArea, toolBar);


    // Stacked views
    ui->pageFav->layout()->setMargin(0);
    ui->pageGallery->layout()->setMargin(0);
    connect(ui->gallery, SIGNAL(mouseDoubleClicked()),
            actToolBarImage, SLOT(trigger()));
    ui->pageImageView->layout()->setMargin(0);

    // PlayList gallery
    // TODO: Lazy load here
    loadSavedPlayLists();
}

void MainWindow::loadSavedPlayLists()
{
    ui->playListGallery->setPlayListRecords(
        PlayListRecord::fromLocalDatabase());
}

void MainWindow::processCommandLineOptions()
{
    QCommandLineParser parser;
    parser.addPositionalArgument("paths", "Directories or files", "[paths...]");

    parser.process(*QCoreApplication::instance());
    const QStringList imagePaths = parser.positionalArguments();

    PlayList playList;
    QStringList urlPatterns = ImageSourceManager::instance()->urlPatterns();
    foreach (const QString& imagePath, imagePaths) {
        QString pattern = imagePath.left(imagePath.indexOf("://"));
        if (urlPatterns.contains(pattern)) {
            playList.addPath(QUrl(imagePath));
        } else {
            playList.addPath(imagePath);
        }
    }

    m_navigator.setPlayList(playList);
}

void MainWindow::promptToOpenImage()
{
    QString defaultDir;
    const QList<QString> &zipDirs = GlobalConfig::instance()->zipDirs();
    if (zipDirs.count() > 0) {
        defaultDir = zipDirs[0];
    }

    QList<QUrl> images = QFileDialog::getOpenFileUrls(
        this, tr("Open"), QUrl::fromLocalFile(defaultDir),
        ImageSourceManager::instance()->fileDialogFilters());

    if (images.count() == 0) {
        return;
    }

    // Hack for zip and 7z
    // FIXME: Better way to handle this?
    for (QList<QUrl>::iterator i = images.begin(); i != images.end(); ++i) {
        if ((*i).path().endsWith(".zip")) {
            (*i).setScheme("zip");
        } else if ((*i).path().endsWith(".7z")) {
            (*i).setScheme("sevenz");
        }
    }

    PlayList playList(images);
    m_navigator.setPlayList(playList);
}

void MainWindow::promptToSaveImage()
{
    if (ui->gallery->scene()->selectedItems().count() > 0) {
        QString destDir = QFileDialog::getExistingDirectory(
            this, "Save to", GlobalConfig::instance()->wallpaperDir());

        if (destDir.isEmpty()) {
            return;
        }

        foreach (QGraphicsItem *item, ui->gallery->scene()->selectedItems()) {
            ImageGalleryItem *imageGalleryItem =
                static_cast<ImageGalleryItem *>(item);
            Image *image = imageGalleryItem->image();
            if (image) {
                QFileInfo imageFile(image->name()); // Remove dir in image name
                QString destPath = destDir + QDir::separator() +
                    imageFile.fileName();
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

void MainWindow::promptToSavePlayList()
{
    QString name = QInputDialog::getText(this, "Save PlayList", "Name");
    Image *image = m_navigator.currentImage();
    if (!name.isEmpty() && image) {
        PlayListRecord record(name, image->thumbnailPath(),
                              m_navigator.playList());
        if (record.saveToLocalDatabase()) {
            statusBar()->showMessage(QString("PlayList %1 saved!").arg(name),
                                     2000);

            // Refresh playlist gallery
            loadSavedPlayLists();
        } else {
            statusBar()->showMessage(
                QString("Failed to save playList %1!").arg(name), 2000);
        }
    }
}

void MainWindow::imageLoaded(Image *image)
{
    // Title bar
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

    // Info widget
    ui->lblName->setText(image->name());
    ui->lblWidth->setText(QString::number(image->data().width()));
    ui->lblHeight->setText(QString::number(image->data().height()));
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
        << (index + 1) << "/" << m_navigator.playList()->count();
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
            promptToOpenImage();
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
        case Qt::Key_S:
            promptToSaveImage();
            break;
        case Qt::Key_P:
            promptToSavePlayList();
            break;
        case Qt::Key_T: {
            QDockWidget *sidebar = ui->sidebar;
            sidebar->setVisible(!sidebar->isVisible());
            break;
        }
        case Qt::Key_R:
            ui->graphicsView->rotate(90);
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
    int visibleWidgetsCount = 0;
    QLayout *layout = ui->centralWidget->layout();
    for (int i = layout->count() - 1; i >=0; --i) {
        QWidget *widget = layout->itemAt(i)->widget();
        if (widget && widget->isVisible()) {
            visibleWidgetsCount ++;
        }
    }

    if (visibleWidgetsCount > 0) {
        int widgetWidth = width() / visibleWidgetsCount;
        for (int i = 0; i < visibleWidgetsCount; ++i) {
            QLayoutItem *panel = ui->centralWidget->layout()->itemAt(i);
            panel->setGeometry(QRect(
                (visibleWidgetsCount - i - 1) * widgetWidth, 0,
                widgetWidth, height()));
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
