#include <QCompleter>
#include <QDesktopWidget>
#include <QDir>
#include <QDirModel>
#include <QDockWidget>
#include <QFileDialog>
#include <QFileInfo>
#include <QGraphicsPixmapItem>
#include <QGridLayout>
#include <QInputDialog>
#include <QLayout>
#include <QMenuBar>
#include <QMessageBox>
#include <QSizePolicy>
#include <QStackedLayout>
#include <QStatusBar>
#include <QToolBar>

#include "image/ImageSourceManager.h"
#include "logger/listeners/ImagePathCorrector.h"
#include "logger/Logger.h"
#include "playlist/LocalPlayListProviderFactory.h"
#include "sapi/LoadingIndicatorDelegate.h"
#include "ui/models/MainGraphicsViewModel.h"
#include "ui/FileSystemItem.h"
#include "ui/ImageGalleryItem.h"
#include "ui/ImagePathCorrectorClientImpl.h"
#include "ui/ImageSourceManagerClientImpl.h"
#include "ui/LoadingIndicator.h"
#include "ui/PlayListGalleryItem.h"
#include "ui/MainMenuBarManager.h"
#include "MainWindow.h"
#include "PlayList.h"
#include "GlobalConfig.h"

#include "ui_MainWindow.h"

#ifdef Q_OS_WIN32
#include "ui/platform/win/D2DMainGraphicsWidget.h"
#endif

static const char* PLAYLIST_TITLE_PREFIX = "PlayList";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) ,
    m_navigator(Navigator::instance()) ,
    m_toolBar(0) ,
    m_toolBarActs(0) ,
    m_actToolBarFS(0) ,
    m_actToolBarFav(0) ,
    m_actToolBarGallery(0) ,
    m_actToolBarImage(0)
{
    ui->setupUi(this);
    setupExtraUi();

    ui->gallery->setPlayList(m_navigator->playList());

    // Side view
    m_sideViewModel.reset(new MainGraphicsViewModel());
    m_sideViewModel->setView(ui->sideView);
    m_sideViewModel->setNavigator(m_navigator);
    ui->sideView->setModel(m_sideViewModel.data());

    ui->sidebar->hide();
    statusBar()->hide();

    // Move to screen center
    const QRect screen = QApplication::desktop()->screenGeometry();
    move(screen.center() - rect().center());

    // Maximize on start
    showMaximized();

    connect(m_navigator, SIGNAL(paint(Image *)),
            this, SLOT(imageLoaded(Image *)));
    connect(m_navigator, SIGNAL(paint(Image *)),
            m_mainGraphicsViewModel.data(), SLOT(paint(Image *)));
    connect(m_navigator, SIGNAL(paintThumbnail(Image *)),
            m_mainGraphicsViewModel.data(), SLOT(paintThumbnail(Image *)));
    connect(m_navigator, SIGNAL(paint(Image *)),
            m_sideViewModel.data(), SLOT(paint(Image *)));
    connect(m_navigator, SIGNAL(paintThumbnail(Image *)),
            m_sideViewModel.data(), SLOT(paintThumbnail(Image *)));

    // StatusBar
    connect(statusBar(), SIGNAL(messageChanged(const QString &)),
            this, SLOT(statusBarMessageChanged(const QString &)));

    // Update sidebar
    connect(m_navigator, SIGNAL(playListChange(PlayList *)),
            this, SLOT(playListChange()));
    connect(m_navigator, SIGNAL(playListAppend(int)),
            this, SLOT(playListAppend(int)));

    // Update gallery
    connect(m_navigator, SIGNAL(playListChange(PlayList *)),
            ui->gallery, SLOT(playListChange(PlayList *)));
    connect(m_navigator, SIGNAL(playListAppend(int)),
            ui->gallery, SLOT(playListAppend(int)));

    connect(m_navigator, SIGNAL(navigationChange(int)),
            this, SLOT(navigationChange(int)));

    // Navigation binding for playListWidget
    connect(ui->playListWidget, SIGNAL(currentRowChanged(int)),
            m_navigator, SLOT(goIndex(int)));

    // Navigation binding for gallery
    connect(ui->gallery->scene(), SIGNAL(selectionChanged()),
            this, SLOT(gallerySelectionChanged()));

    processCommandLineOptions();
}

MainWindow::~MainWindow()
{
    QSettings setting;
    setting.setValue("LAST_FILE_SYSTEM_PATH", ui->fsView->rootPath());

    Logger::instance()->removeListener(m_imagePathCorrector);
    delete m_imagePathCorrector;

    delete ui;
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

    // Main graphics view
    m_mainGraphicsViewModel.reset(new MainGraphicsViewModel());
    m_mainGraphicsViewModel->setNavigator(m_navigator);

    ui->graphicsView->deleteLater();
#ifdef Q_OS_WIN32
    D2DMainGraphicsWidget *mainGraphicsView = new D2DMainGraphicsWidget(ui->pageImageView);
#else
    MainGraphicsView *mainGraphicsView = new MainGraphicsView(ui->pageImageView);
#endif
    ui->pageImageView->layout()->addWidget(mainGraphicsView);
    mainGraphicsView->setModel(m_mainGraphicsViewModel.data());
    m_mainGraphicsViewModel->setView(mainGraphicsView);
    ui->graphicsView = mainGraphicsView;

    // Main menu bar
    MainMenuBarManager::Context menuBarCtx;
    menuBarCtx.menuBar = menuBar();
    menuBarCtx.navigator = m_navigator;
    menuBarCtx.imageView = ui->graphicsView;

    new MainMenuBarManager(menuBarCtx, this);

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

    // File system icon
    QIcon toolbarFSIcon(":/res/toolbar/fs.png");
    toolbarFSIcon.addFile(":/res/toolbar/fs-active.png",
                          QSize(), QIcon::Active, QIcon::On);
    m_actToolBarFS = m_toolBar->addAction(
        toolbarFSIcon, tr("File System"), toolBarSigMapper, SLOT(map()));
    m_actToolBarFS->setCheckable(true);
    m_actToolBarFS->setShortcut(Qt::CTRL + Qt::Key_2);
    m_toolBarActs->addAction(m_actToolBarFS);
    toolBarSigMapper->setMapping(m_actToolBarFS, 1);

    // Gallery icon
    QIcon toolBarGalleryIcon(":/res/toolbar/gallery.png");
    toolBarGalleryIcon.addFile(":/res/toolbar/gallery-active.png",
                               QSize(), QIcon::Active, QIcon::On);
    m_actToolBarGallery = m_toolBar->addAction(
        toolBarGalleryIcon, tr("Gallery"), toolBarSigMapper, SLOT(map()));
    m_actToolBarGallery->setCheckable(true);
    m_actToolBarGallery->setShortcut(Qt::CTRL + Qt::Key_3);
    m_toolBarActs->addAction(m_actToolBarGallery);
    toolBarSigMapper->setMapping(m_actToolBarGallery, 2);

    // Image icon
    QIcon toolBarImageIcon(":/res/toolbar/image-view.png");
    toolBarImageIcon.addFile(":/res/toolbar/image-view-active.png",
                             QSize(), QIcon::Active, QIcon::On);
    m_actToolBarImage = m_toolBar->addAction(
        toolBarImageIcon, tr("Image"), toolBarSigMapper, SLOT(map()));
    m_actToolBarImage->setShortcut(Qt::CTRL + Qt::Key_4);
    m_actToolBarImage->setCheckable(true);
    m_toolBarActs->addAction(m_actToolBarImage);
    toolBarSigMapper->setMapping(m_actToolBarImage, 3);

    // Fav view is the default
    m_actToolBarFav->setChecked(true);
    addToolBar(Qt::LeftToolBarArea, m_toolBar);

    // Toolbar spacing
    QWidget *toolBarSpacing = new QWidget(m_toolBar);
    toolBarSpacing->setSizePolicy(
        QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding));
    m_toolBar->addWidget(toolBarSpacing);

    // Loading indicator
    LoadingIndicator *loadInd = new LoadingIndicator(QSize(25, 25), m_toolBar);
    sapi::initPluginLoadingIndicator(loadInd);
    QWidget *loadIndWrapper = new QWidget(m_toolBar);
    loadIndWrapper->setMinimumSize(loadInd->minimumSize());
    loadIndWrapper->setLayout(new QGridLayout(loadIndWrapper));
    loadIndWrapper->layout()->setContentsMargins(0, 0, 0, 5);
    loadIndWrapper->layout()->addWidget(loadInd);
    m_toolBar->addWidget(loadIndWrapper);
    loadInd->addTaskProgress(ui->gallery->loadProgress());
    loadInd->addTaskProgress(ui->gallery->groupingProgress());
    loadInd->addTaskProgress(ui->fsView->loadProgress());

    // Stacked views
    ui->pageFav->layout()->setMargin(0);
    connect(ui->playListGallery, SIGNAL(mouseDoubleClicked()),
            this, SLOT(loadSelectedPlayList()));
    connect(ui->playListGallery, SIGNAL(keyEnterPressed()),
            this, SLOT(loadSelectedPlayList()));

    // Fils system tab toolbar
    ui->pageFileSystemLayout->setMargin(0);
    ui->pageFileSystemLayout->setSpacing(0);
    ui->fsToolBar->layout()->setSpacing(15);
    ui->fsToolBar->layout()->setAlignment(Qt::AlignLeft);
    connect(ui->fsBtnCdUp, SIGNAL(clicked()),
            ui->fsView, SLOT(cdUp()));
    connect(ui->fsEditPath, SIGNAL(editingFinished()),
            this, SLOT(fsRootPathChanged()));

    // File system view
    connect(ui->fsView, SIGNAL(mouseDoubleClicked()),
            this, SLOT(loadOrEnterSelectedPath()));
    connect(ui->fsView, SIGNAL(keyEnterPressed()),
            this, SLOT(loadSelectedPath()));
    connect(ui->fsView, SIGNAL(rootPathChanged(QString)),
            ui->fsEditPath, SLOT(setText(QString)));
    QCompleter *fsEditPathComp = new QCompleter(ui->fsEditPath);
    fsEditPathComp->setModel(new QDirModel(fsEditPathComp));
    fsEditPathComp->setCaseSensitivity(Qt::CaseInsensitive);
    ui->fsEditPath->setCompleter(fsEditPathComp);
    QSettings setting;
    QString fsRootPath = setting.value("LAST_FILE_SYSTEM_PATH",
                                       QDir::homePath()).toString();
    ui->fsView->setRootPath(fsRootPath);

    ui->pageGallery->layout()->setMargin(0);
    ui->pageGallery->layout()->setSpacing(0);
    ui->sideView->hide();
    connect(ui->gallery, SIGNAL(mouseDoubleClicked()),
            m_actToolBarImage, SLOT(trigger()));
    connect(ui->gallery, SIGNAL(keyEnterPressed()),
            m_actToolBarImage, SLOT(trigger()));
    connect(m_mainGraphicsViewModel.data(), SIGNAL(mouseDoubleClicked()),
            m_actToolBarGallery, SLOT(trigger()));

    ui->pageImageView->layout()->setMargin(0);


    // PlayList gallery
    connect(ui->playListGallery, SIGNAL(promptToCreatePlayListRecord(int)),
            this, SLOT(promptToCreatePlayListRecord(int)));

    // Init Basket
    ui->basketPane->hide();
    ui->basketPane->layout()->setSpacing(0);
    ui->basketPane->setMaximumHeight(qApp->desktop()->geometry().height() / 3);
    ui->basketView->setPlayList(m_navigator->basket());
    connect(m_navigator->basket(), SIGNAL(itemsAppended(int)),
            ui->basketView, SLOT(playListAppend(int)));
    connect(m_navigator->basket(), SIGNAL(itemsChanged()),
            ui->basketView, SLOT(playListChange()));

    // Init image source manager client
    ImageSourceManager::instance()->setClient(
        new ImageSourceManagerClientImpl(this));


    // Init image path corrector client
    ImagePathCorrector::PromptClient *imagePathCorrectorClientImpl =
        new ImagePathCorrectorClientImpl(this);
    m_imagePathCorrector =
        new ImagePathCorrector(GlobalConfig::instance()->searchDirs(),
                               m_navigator, imagePathCorrectorClientImpl);
    Logger::instance()->addListener(Logger::IMAGE_LOAD_ERROR,
                                    m_imagePathCorrector);
    Logger::instance()->addListener(Logger::IMAGE_THUMBNAIL_LOAD_ERROR,
                                    m_imagePathCorrector);

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
    QList<GalleryItem *> selectedItems =
        ui->playListGallery->selectedGalleryItems();
    if (selectedItems.count() > 0) {
        // TODO(sdy): Support multiple playlists
        //
        // Watching option for PlayList::append() brought too many hidden
        // bugs. Need to figure out new way to support viewing multiple
        // playlists. One PlayListRecord with a list of providers may be
        // a choice?
        PlayListGalleryItem *playListItem =
            static_cast<PlayListGalleryItem *>(selectedItems[0]);
        PlayList *pl = playListItem->record()->playList();
        m_navigator->setPlayList(pl);

        // Navigator should take ownership of PlayList in this case
        // m_navigator->setPlayList(pl, true);

        // Make cover image of first playlist item selected
        PlayListGalleryItem *firstItem =
            static_cast<PlayListGalleryItem *>(selectedItems[0]);
        int coverIndex = firstItem->record()->coverIndex();

        QList<GalleryItem *> items = ui->gallery->galleryItems();
        if (coverIndex >= 0 && coverIndex < items.count()) {
            GalleryItem *coverImageItem = items.at(coverIndex);
            ui->gallery->scene()->clearSelection();

            // This has to be async because QGraphicsItem::setSelected() will
            // have no effect if item is not visible while our item will remain
            // hide until thumbnail loaded.
            coverImageItem->scheduleSelectedAfterShown();
        }

        // Show gallery view
        m_actToolBarGallery->trigger();
    }
}

void MainWindow::loadOrEnterSelectedPath()
{
    QList<GalleryItem *> selectedItems = ui->fsView->selectedGalleryItems();
    if (selectedItems.count() > 0) {
        FileSystemItem *firstItem =
            static_cast<FileSystemItem *>(selectedItems[0]);
        if (firstItem->fileInfo().isDir()) {
            // If first selected item is a directory, navigate into it
            ui->fsView->setRootPath(firstItem->path());
        } else {
            // Load all selected items
            loadSelectedPath();
        }
    }
}

void MainWindow::loadSelectedPath()
{
    QList<GalleryItem *> selectedItems = ui->fsView->selectedGalleryItems();
    if (selectedItems.count() > 0) {
        PlayList *pl = new PlayList();

        foreach (GalleryItem *item, selectedItems) {
            FileSystemItem *fsItem = static_cast<FileSystemItem *>(item);
            pl->addMultiplePath(fsItem->path());
        }

        // If only one item selected and only one image in playlist,
        // add siblings too
        if (selectedItems.count() == 1 && pl->count() == 1) {
            FileSystemItem *fsItem =
                static_cast<FileSystemItem *>(selectedItems[0]);
            QDir curDir = fsItem->fileInfo().dir();
            QFileInfoList entries = curDir.entryInfoList(
                ImageSourceManager::instance()->nameFilters(),
                QDir::Files | QDir::NoDotAndDotDot,
                QDir::Name);
            foreach (const QFileInfo &info, entries) {
                // Avoid duplicate
                if (info.absoluteFilePath() != fsItem->path()) {
                    pl->addSinglePath(info.absoluteFilePath());
                }
            }
        } else {
            pl->sortByName();
        }

        // Empty playlist will not be loaded
        if (pl->count() == 0) {
            return;
        }

        // Navigator should take ownership of PlayList in this case
        m_navigator->setPlayList(pl, true);

        if (selectedItems.count() > 1) {
            // Show gallery view
            m_actToolBarGallery->trigger();
        } else {
            // Show image view
            m_actToolBarImage->trigger();
        }
    }
}

void MainWindow::processCommandLineOptions()
{
    QCommandLineParser parser;
    parser.addPositionalArgument("paths", "Directories or files", "[paths...]");

    parser.process(*QCoreApplication::instance());
    const QStringList imagePaths = parser.positionalArguments();

    PlayList *playList = new PlayList();
    QStringList urlPatterns = ImageSourceManager::instance()->urlPatterns();
    foreach (const QString& imagePath, imagePaths) {
        QString pattern = imagePath.left(imagePath.indexOf("://"));
        if (urlPatterns.contains(pattern)) {
            playList->addMultiplePath(QUrl(imagePath));
        } else {
            playList->addMultiplePath(imagePath);
        }
    }

    m_navigator->setPlayList(playList, true);
}

void MainWindow::promptToOpenImage()
{
    static QString defaultDir;
    if (defaultDir.isEmpty()) {
        const QList<QString> &zipDirs = GlobalConfig::instance()->searchDirs();
        if (zipDirs.count() > 0) {
            defaultDir = zipDirs[0];
        }
    }

    QList<QUrl> images = QFileDialog::getOpenFileUrls(
        this, tr("Open"), QUrl::fromLocalFile(defaultDir),
        ImageSourceManager::instance()->fileDialogFilters());

    if (images.count() == 0) {
        return;
    }

    // Record dir for next open
    QFileInfo firstFileInfo(images[0].toLocalFile());
    defaultDir = firstFileInfo.absoluteDir().path();

    // Hack for zip and 7z
    // FIXME: Better way to handle this?
    for (QList<QUrl>::iterator i = images.begin(); i != images.end(); ++i) {
        if ((*i).path().endsWith(".zip")) {
            (*i).setScheme("zip");
        } else if ((*i).path().endsWith(".7z")) {
            (*i).setScheme("sevenz");
        } else if ((*i).path().endsWith(".rar")) {
            (*i).setScheme("rar");
        }
    }

    PlayList *playList = new PlayList(images);
    // Navigator takes ownership of PlayList in this case
    m_navigator->setPlayList(playList, true);
}

void MainWindow::promptToOpenDir()
{
    static QString lastParentDir;

    QString dir = QFileDialog::getExistingDirectory(
    this, tr("Open Folder"), lastParentDir);
    if (dir.isEmpty()) {
        return;
    }

    // Record parent dir for next open
    QDir lastDir(dir);
    lastDir.cdUp();
    lastParentDir = lastDir.absolutePath();

    QList<QUrl> images;
    QDirIterator iter(dir, ImageSourceManager::instance()->nameFilters(),
                   QDir::Files, QDirIterator::Subdirectories);
    while (iter.hasNext()) {
        QString path = iter.next();
        QUrl url = QUrl::fromLocalFile(path);

        // Hack for zip and 7z
        // FIXME: Better way to handle this?
        if (url.path().endsWith(".zip")) {
         url.setScheme("zip");
        } else if (url.path().endsWith(".7z")) {
         url.setScheme("sevenz");
        } else if (url.path().endsWith(".rar")) {
         url.setScheme("rar");
        }

        images << url;
    }

    PlayList *playList = new PlayList(images);
    // Navigator takes ownership of PlayList in this case
    m_navigator->setPlayList(playList, true);
}

void MainWindow::promptToSaveImage()
{
    QList<GalleryItem *> selectedGalleryItems =
        ui->gallery->selectedGalleryItems();
    if (selectedGalleryItems.count() > 0) {
        QString destDir = QFileDialog::getExistingDirectory(
            this, "Save to", GlobalConfig::instance()->wallpaperDir());

        if (destDir.isEmpty()) {
            return;
        }

        foreach (GalleryItem *item, selectedGalleryItems) {
            ImageGalleryItem *imageGalleryItem =
                static_cast<ImageGalleryItem *>(item);
            ImagePtr image = imageGalleryItem->image();
            if (!image.isNull()) {
                QFileInfo imageFile(image->name()); // Remove dir in image name
                QString destPath = destDir + "/" +
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

void MainWindow::promptToSaveLocalPlayList()
{
    Image *image = m_navigator->currentImage();
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
    if (dialog.result() == QDialog::Accepted && !name.isEmpty()) {
        PlayListRecordBuilder plrBuilder;
        plrBuilder
            .setName(name)
            .setCoverPath(image->thumbnailPath())
            .setPlayList(m_navigator->playList())
            .setType(LocalPlayListProviderFactory::TYPE);
        PlayListRecord *record = plrBuilder.obtain();
        if (record->save()) {
            statusBar()->showMessage(QString("PlayList %1 saved!").arg(name),
                                     2000);

            // Refresh playlist gallery
            loadSavedPlayLists();
        } else {
            statusBar()->showMessage(
                QString("Failed to save playList %1!").arg(name), 2000);
        }
        delete record;
    }
}

void MainWindow::promptToCreatePlayListRecord(int type)
{
    QString name = QInputDialog::getText(this, "New PlayList", "Name");
    if (!name.isEmpty()) {
        PlayListRecordBuilder plrBuilder;
        plrBuilder
            .setName(name)
            .setType(type);
        PlayListRecord *record = plrBuilder.obtain();
        if (record->save()) {
            statusBar()->showMessage(QString("PlayList %1 saved!").arg(name),
                                     2000);

            // Refresh playlist gallery
            loadSavedPlayLists();
        } else {
            statusBar()->showMessage(
                QString("Failed to save playList %1!").arg(name), 2000);
        }
        delete record;
    }
}

void MainWindow::fsRootPathChanged()
{
    ui->fsView->setRootPath(ui->fsEditPath->text());
}

void MainWindow::showAbout()
{
    QString text;
    QTextStream builder(&text);
    builder << "Build Revision: "
            << GlobalConfig::instance()->buildRevision() << "\n"
            << "Build Timestamp: "
            << GlobalConfig::instance()->buildTimestamp();
    QMessageBox::about(this, tr("About Silica"), text);
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
        << (m_navigator->currentIndex() + 1) << "/"
        << m_navigator->playList()->count() << "] ";

    QString title;
    QTextStream(&title) << status << progress << image->name();
    setWindowTitle(title);

    // Info widget
    ui->lblName->setText(image->name());
    ui->lblWidth->setText(QString::number(image->data().width()));
    ui->lblHeight->setText(QString::number(image->data().height()));
}

void MainWindow::playListChange()
{
    // Switch to gallery view if playlist is not empty
    if (!m_navigator->playList()->isEmpty()) {
        m_actToolBarGallery->trigger();
    }

    ui->playListWidget->clear();

    playListAppend(0);
}

void MainWindow::playListAppend(int start)
{
    PlayList *pl = m_navigator->playList();

    QListWidget *list = ui->playListWidget;
    for (int i = start; i < pl->count(); ++i) {
        list->addItem(pl->at(i).data()->name());
    }

    updateSidebarTitle();
}

void MainWindow::navigationChange(int index)
{
    // PlayList widget
    ui->playListWidget->setCurrentRow(index);

    // Gallery widget
    QList<GalleryItem *> galleryItems = ui->gallery->galleryItems();
    if (index >= 0 && index < galleryItems.count()) {
        QGraphicsItem *selectedItem = galleryItems.at(index);
        if (selectedItem) {
            ui->gallery->scene()->clearSelection();
            selectedItem->setSelected(true);
            selectedItem->ensureVisible();
        }
    }

    // Sidebar
    updateSidebarTitle();
}

void MainWindow::updateSidebarTitle()
{
    int index = m_navigator->currentIndex();
    QString title;
    QTextStream(&title) << PLAYLIST_TITLE_PREFIX << " - "
        << (index + 1) << "/" << m_navigator->playList()->count();
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
    QList<GalleryItem *> selectedGalleryItems =
        ui->gallery->selectedGalleryItems();
    if (selectedGalleryItems.length() > 0) {
        GalleryItem *item = selectedGalleryItems[0];
        QList<GalleryItem *> allItems = ui->gallery->galleryItems();
        int index = allItems.indexOf(item);
        m_navigator->goIndex(index);
    }
}

void MainWindow::keyPressEvent(QKeyEvent *ev)
{
    if (ev->modifiers() == Qt::NoModifier) {
        switch (ev->key()) {
            case Qt::Key_J:
            case Qt::Key_Space:
                if (m_navigator->isAutoNavigating()) {
                    m_navigator->stopAutoNavigation();
                } else {
                    m_navigator->goNext();
                }
                break;
            case Qt::Key_K:
                if (m_navigator->isAutoNavigating()) {
                    m_navigator->stopAutoNavigation();
                } else {
                    m_navigator->goPrev();
                }
                break;
            case Qt::Key_L:
                m_navigator->goNextGroup();
                break;
            case Qt::Key_H:
                m_navigator->goPrevGroup();
                break;
            case Qt::Key_F:
                m_mainGraphicsViewModel->toggleFitInView();
                m_mainGraphicsViewModel->fitInViewIfNecessary();
                break;
            case Qt::Key_O: {
                promptToOpenImage();
                break;
            }
            case Qt::Key_S:
                promptToSaveImage();
                break;
            case Qt::Key_P:
                promptToSaveLocalPlayList();
                break;
            case Qt::Key_G:
                switchViews();
                break;
            case Qt::Key_Backslash:
                ui->sideView->isVisible() ?
                    ui->sideView->hide() :
                    ui->sideView->show();
                break;
            case Qt::Key_T: {
                QDockWidget *sidebar = ui->sidebar;
                sidebar->setVisible(!sidebar->isVisible());
                break;
            }
            case Qt::Key_R:
                m_mainGraphicsViewModel->rotate(90);
                break;
            case Qt::Key_B:
                ui->basketPane->setVisible(!ui->basketPane->isVisible());
                break;
            case Qt::Key_F1:
                showAbout();
                break;
            case Qt::Key_F10:
                if (!isFullScreen()) {
                    setWindowState(Qt::WindowFullScreen);
                } else {
                    setWindowState(Qt::WindowNoState);
                }
                break;
            case Qt::Key_Escape: {
                if (QMessageBox::question(this, "Exit", "Exit?") ==
                        QMessageBox::Yes) {
                    QApplication::exit();
                }
                break;
            }
            case Qt::Key_Home:
                m_navigator->goFirst();
                break;
            case Qt::Key_End:
                m_navigator->goLast();
                break;
            case Qt::Key_Period:
                m_navigator->startAutoNavigation(Navigator::NormalDirection);
                break;
            case Qt::Key_Comma:
                m_navigator->startAutoNavigation(Navigator::ReverseDirection);
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
                m_mainGraphicsViewModel->fitGridInView(ev->text().toInt());
                break;
        }

        ev->accept();
    } else if (ev->modifiers() == Qt::ShiftModifier) {
        switch (ev->key()) {
        case Qt::Key_O:
            promptToOpenDir();
            break;
        }

        ev->accept();
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
    m_mainGraphicsViewModel->fitInViewIfNecessary();
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
