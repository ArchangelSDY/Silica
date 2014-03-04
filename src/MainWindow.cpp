#include <QDesktopWidget>
#include <QDockWidget>
#include <QFileDialog>
#include <QGraphicsPixmapItem>
#include <QInputDialog>
#include <QMessageBox>
#include <QStackedLayout>
#include <QStatusBar>
#include <QToolBar>

#include "AsunaDatabase.h"
#include "GlobalConfig.h"
#include "ImageGalleryItem.h"
#include "ImageSourceManager.h"
#include "PlayList.h"
#include "PlayListGalleryItem.h"
#include "PlayListRecord.h"
#include "MainWindow.h"

#include "ui_MainWindow.h"

static const char* PLAYLIST_TITLE_PREFIX = "PlayList";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) ,
    m_toolBar(0) ,
    m_toolBarActs(0) ,
    m_actToolBarFav(0) ,
    m_actToolBarGallery(0) ,
    m_actToolBarImage(0) ,
    m_database(new AsunaDatabase()) ,
    m_commandInterpreter(&m_navigator, m_database) ,
    m_inputMode(InputMode_Control)
{
    ui->setupUi(this);
    setupExtraUi();

    ui->gallery->setNavigator(&m_navigator);
    ui->graphicsView->setNavigator(&m_navigator);
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

    // Navigation binding for gallery
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
    m_toolBar = new QToolBar(this);
    m_toolBar->setFloatable(false);
    m_toolBar->setMovable(false);
    m_toolBar->setOrientation(Qt::Vertical);
    m_toolBar->setIconSize(QSize(24, 24));
    m_toolBar->setStyleSheet(
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

    m_toolBarActs = new QActionGroup(this);
    QSignalMapper *toolBarSigMapper = new QSignalMapper(this);
    connect(toolBarSigMapper, SIGNAL(mapped(int)),
            ui->stackedViews, SLOT(setCurrentIndex(int)));

    // Fav icon
    QIcon toolBarFavIcon(":/res/toolbar/fav.png");
    toolBarFavIcon.addFile(":/res/toolbar/fav-active.png",
                           QSize(), QIcon::Active, QIcon::On);
    m_actToolBarFav = m_toolBar->addAction(
        toolBarFavIcon, tr("Favourite"), toolBarSigMapper, SLOT(map()));
    m_actToolBarFav->setCheckable(true);
    m_actToolBarFav->setShortcut(Qt::CTRL + Qt::Key_1);
    m_toolBarActs->addAction(m_actToolBarFav);
    toolBarSigMapper->setMapping(m_actToolBarFav, 0);

    // Gallery icon
    QIcon toolBarGalleryIcon(":/res/toolbar/gallery.png");
    toolBarGalleryIcon.addFile(":/res/toolbar/gallery-active.png",
                               QSize(), QIcon::Active, QIcon::On);
    m_actToolBarGallery = m_toolBar->addAction(
        toolBarGalleryIcon, tr("Gallery"), toolBarSigMapper, SLOT(map()));
    m_actToolBarGallery->setCheckable(true);
    m_actToolBarGallery->setShortcut(Qt::CTRL + Qt::Key_2);
    m_toolBarActs->addAction(m_actToolBarGallery);
    toolBarSigMapper->setMapping(m_actToolBarGallery, 1);

    // Image icon
    QIcon toolBarImageIcon(":/res/toolbar/image-view.png");
    toolBarImageIcon.addFile(":/res/toolbar/image-view-active.png",
                             QSize(), QIcon::Active, QIcon::On);
    m_actToolBarImage = m_toolBar->addAction(
        toolBarImageIcon, tr("Image"), toolBarSigMapper, SLOT(map()));
    m_actToolBarImage->setShortcut(Qt::CTRL + Qt::Key_3);
    m_actToolBarImage->setCheckable(true);
    m_toolBarActs->addAction(m_actToolBarImage);
    toolBarSigMapper->setMapping(m_actToolBarImage, 2);

    // Fav view is the default
    m_actToolBarFav->setChecked(true);
    addToolBar(Qt::LeftToolBarArea, m_toolBar);


    // Stacked views
    ui->pageFav->layout()->setMargin(0);
    connect(ui->playListGallery, SIGNAL(mouseDoubleClicked()),
            this, SLOT(loadSelectedPlayList()));
    connect(ui->playListGallery, SIGNAL(mouseDoubleClicked()),
            m_actToolBarGallery, SLOT(trigger()));

    ui->pageGallery->layout()->setMargin(0);
    connect(ui->gallery, SIGNAL(mouseDoubleClicked()),
            m_actToolBarImage, SLOT(trigger()));
    connect(ui->graphicsView, SIGNAL(mouseDoubleClicked()),
            m_actToolBarGallery, SLOT(trigger()));

    ui->pageImageView->layout()->setMargin(0);


    // Image view

    // QStackedViews won't layout hidden views but paintThumbnail happens
    // before showEvent so that view size is incorretly small.
    //
    // By setting stackingMode to StackAll, we force all stacked views to be
    // visible so that their layouts are correctly set.
    QStackedLayout *stackedLayout =
        static_cast<QStackedLayout *>(ui->stackedViews->layout());
    stackedLayout->setStackingMode(QStackedLayout::StackAll);

    // PlayList gallery
    // TODO: Lazy load here
    loadSavedPlayLists();
}

void MainWindow::loadSavedPlayLists()
{
    ui->playListGallery->setPlayListRecords(
        PlayListRecord::all());
}

void MainWindow::loadSelectedPlayList()
{
    if (ui->playListGallery->scene()->selectedItems().count() > 0) {
        PlayListGalleryItem *playListItem = static_cast<PlayListGalleryItem *>(
            ui->playListGallery->scene()->selectedItems()[0]);
        m_navigator.setPlayList(*playListItem->record()->playList());

        // Select cover image
        const QList<QGraphicsItem *> &galleryItems =
            ui->gallery->scene()->items(Qt::AscendingOrder);
        int coverIndex = playListItem->record()->coverIndex();
        if (coverIndex >=0 && coverIndex < galleryItems.count()) {
            QGraphicsItem *coverImageItem = galleryItems.at(coverIndex);
            ui->gallery->scene()->clearSelection();
            coverImageItem->setSelected(true);
        }
    }
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
    Image *image = m_navigator.currentImage();
    if (!image) {
        return;
    }

    QInputDialog dialog(this);
    dialog.setWindowTitle("Save PlayList");
    dialog.setLabelText("Name");
    dialog.setComboBoxEditable(true);

    QString sourceUrl = image->source()->url().toString();
    QStringList defaultNames = sourceUrl.split(QRegularExpression("[/#]"));
    dialog.setComboBoxItems(defaultNames);

    if (defaultNames.count() >= 2) {
        dialog.setTextValue(defaultNames[defaultNames.count() - 2]);
    }

    dialog.exec();

    QString name = dialog.textValue();
    if (!name.isEmpty()) {
        PlayListRecord record(name, image->thumbnailPath(),
                              m_navigator.playList());
        if (record.save()) {
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

    QString progress;
    QTextStream(&progress) << "["
        << (m_navigator.currentIndex() + 1) << "/"
        << m_navigator.playList()->count() << "] ";

    QString title;
    QTextStream(&title) << status << progress << image->name();
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

    // Switch to gallery view if playlist is not empty
    if (!playList.isEmpty()) {
        m_actToolBarGallery->trigger();
    }
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
    if (ev->modifiers() == Qt::NoModifier) {
        switch (ev->key()) {
            case Qt::Key_J:
            case Qt::Key_Space:
                if (m_navigator.isAutoNavigating()) {
                    m_navigator.stopAutoNavigation();
                } else {
                    m_navigator.goNext();
                }
                break;
            case Qt::Key_K:
                if (m_navigator.isAutoNavigating()) {
                    m_navigator.stopAutoNavigation();
                } else {
                    m_navigator.goPrev();
                }
                break;
            case Qt::Key_F:
                ui->graphicsView->toggleFitInView();
                ui->graphicsView->fitInViewIfNecessary();
                break;
            case Qt::Key_O: {
                promptToOpenImage();
                break;
            }
            case Qt::Key_S:
                promptToSaveImage();
                break;
            case Qt::Key_P:
                promptToSavePlayList();
                break;
            case Qt::Key_G:
                switchViews();
                break;
            case Qt::Key_T: {
                QDockWidget *sidebar = ui->sidebar;
                sidebar->setVisible(!sidebar->isVisible());
                break;
            }
            case Qt::Key_R:
                ui->graphicsView->rotate(90);
                break;
            case Qt::Key_F10:
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
            case Qt::Key_Period:
                m_navigator.startAutoNavigation(Navigator::NormalDirection);
                break;
            case Qt::Key_Comma:
                m_navigator.startAutoNavigation(Navigator::ReverseDirection);
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

void MainWindow::switchViews()
{
    QAction *currentAct = m_toolBarActs->checkedAction();
    int currentIndex = m_toolBarActs->actions().indexOf(currentAct);
    int nextIndex = (currentIndex + 1) % m_toolBarActs->actions().count();
    QAction *nextAct = m_toolBarActs->actions()[nextIndex];
    nextAct->trigger();
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    ui->graphicsView->fitInViewIfNecessary();
}

void MainWindow::changeEvent(QEvent *ev)
{
    if (ev->type() == QEvent::WindowStateChange) {
        if (isFullScreen()) {
            m_toolBar->hide();
        } else {
            m_toolBar->show();
        }

        ev->accept();
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
