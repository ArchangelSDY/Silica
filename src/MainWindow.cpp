#include <QCompleter>
#include <QDir>
#include <QDockWidget>
#include <QFileDialog>
#include <QFileInfo>
#include <QFileSystemModel>
#include <QFutureWatcher>
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
#include <QtConcurrent>

#include "db/LocalDatabase.h"
#include "image/caches/ImagesCache.h"
#include "image/caches/LoopImagesCacheStrategy.h"
#include "image/effects/ImageEffectManager.h"
#include "image/ImageSourceManager.h"
#include "logger/listeners/ImagePathCorrector.h"
#include "logger/Logger.h"
#include "navigation/NavigationPlayerManager.h"
#include "navigation/NormalNavigationPlayer.h"
#include "playlist/sort/PlayListImageUrlSorter.h"
#include "playlist/FileSystemPlayListProvider.h"
#include "playlist/LocalPlayListProvider.h"
#include "playlist/PlayListProviderManager.h"
#include "playlist/PlayListProvider.h"
#include "sapi/LoadingIndicatorDelegate.h"
#include "share/SharerManager.h"
#include "ui/models/MainGraphicsViewModel.h"
#include "ui/playList/FileSystemPlayListProviderView.h"
#include "ui/playList/PlayListProviderViewManager.h"
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

#ifdef ENABLE_OPENGL
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLExtraFunctions>
#endif

#ifdef Q_OS_WIN32
#include "ui/platform/win/D2DMainGraphicsWidget.h"
#include "ui/platform/win/DirectXHelper.h"
#endif

static const char* PLAYLIST_TITLE_PREFIX = "PlayList";

// Cache both backward/forward preloaded images and the current one
static const int MAX_CACHE = 2 * Navigator::MAX_PRELOAD + 1;

CurrentPlayListEntityState::CurrentPlayListEntityState(
    QSharedPointer<PlayListEntity> e,
    QSharedPointer<PlayListEntityLoadContext> c) : entity(e), loadContext(c)
{
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_currentPlayListProvider(nullptr) ,
    m_imagesCache(new ImagesCache(MAX_CACHE)) ,
    m_navigator(new Navigator(m_imagesCache, LocalDatabase::instance())),
    m_secondaryNavigator(new Navigator(m_imagesCache, LocalDatabase::instance())) ,
    m_secondaryMainGraphicsViewModel(nullptr) ,
    m_secondaryMainGraphicsView(nullptr) ,
    m_navigatorSynchronizer(m_navigator, m_secondaryNavigator.data()) ,
    m_toolBar(0) ,
    m_toolBarActs(0) ,
    m_actToolBarFS(0) ,
    m_actToolBarFav(0) ,
    m_actToolBarGallery(0) ,
    m_actToolBarImage(0) ,
    m_imageEffectManager(new ImageEffectManager()) ,
    m_keyState(KEY_STATE_NORMAL)
{
    ui->setupUi(this);
    setupExtraUi();

    // TODO
    // ui->gallery->setPlayList(m_navigator->playList());

    // Side view
    m_sideViewModel.reset(new MainGraphicsViewModel(m_navigator, m_imageEffectManager.data(), nullptr));
    m_sideViewModel->setView(ui->sideView);
    ui->sideView->setModel(m_sideViewModel.data());

    ui->sidebar->hide();
    statusBar()->hide();

    // Move to screen center
    QScreen *primaryScreen = QApplication::primaryScreen();
    QRect primaryScreenRect = primaryScreen->geometry();
    move(primaryScreenRect.center() - rect().center());

    // Maximize on start
    showMaximized();

    connect(m_navigator, &Navigator::paint, this, &MainWindow::imageLoaded);

    connect(m_navigator, &Navigator::navigationChange, m_mainGraphicsViewModel.data(), &MainGraphicsViewModel::reset);
    connect(m_navigator, &Navigator::paint, m_mainGraphicsViewModel.data(), &MainGraphicsViewModel::setImage);
    connect(m_navigator, &Navigator::paintThumbnail, m_mainGraphicsViewModel.data(), &MainGraphicsViewModel::setThumbnail);

    connect(m_navigator, &Navigator::navigationChange, m_sideViewModel.data(), &MainGraphicsViewModel::reset);
    connect(m_navigator, &Navigator::paint, m_sideViewModel.data(), &MainGraphicsViewModel::setImage);
    connect(m_navigator, &Navigator::paintThumbnail, m_sideViewModel.data(), &MainGraphicsViewModel::setThumbnail);

    connect(m_navigator, &Navigator::continueProvide, this, &MainWindow::continuePlayList);

    // StatusBar
    connect(statusBar(), SIGNAL(messageChanged(const QString &)),
            this, SLOT(statusBarMessageChanged(const QString &)));

    // Update sidebar
    connect(m_navigator, SIGNAL(playListChange(QSharedPointer<PlayList>)),
            this, SLOT(playListChange()));
    connect(m_navigator, SIGNAL(playListAppend(int)),
            this, SLOT(playListAppend(int)));

    // Update gallery
    connect(m_navigator, &Navigator::playListItemChange, ui->gallery, &MainImageGalleryView::playListItemChange);
    connect(m_navigator, &Navigator::playListChange, ui->gallery, &MainImageGalleryView::setPlayList);
    connect(m_navigator, &Navigator::playListAppend, ui->gallery, &MainImageGalleryView::playListAppend);

    connect(m_navigator, SIGNAL(navigationChange(int)),
            this, SLOT(navigationChange(int)));

    // Navigation binding for playListWidget
    connect(ui->playListWidget, SIGNAL(currentRowChanged(int)),
            m_navigator, SLOT(goIndex(int)));
    connect(ui->playListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem *)),
            this, SLOT(editImageUrl(QListWidgetItem *)));

    // Navigation binding for gallery
    connect(ui->gallery->scene(), SIGNAL(selectionChanged()),
            this, SLOT(gallerySelectionChanged()));

    // Gamepad
    m_gamepadController.reset(new GamepadController(this));

    processCommandLineOptions();
}

MainWindow::~MainWindow()
{
    auto fsProvider = static_cast<FileSystemPlayListProvider *>(PlayListProviderManager::instance()->get(FileSystemPlayListProvider::TYPE));
    QSettings setting;
    setting.setValue("LAST_FILE_SYSTEM_PATH", fsProvider->rootPath());

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
    ui->graphicsView->deleteLater();
    m_mainGraphicsViewModel.reset(new MainGraphicsViewModel(m_navigator, m_imageEffectManager.data(), &m_basket));
    createMainImageView(&ui->graphicsView, ui->pageImageView, m_mainGraphicsViewModel.data());

    ui->pageImageViewLayout->setSpacing(0);

    // Use normal player by default
    m_navigator->setPlayer(NavigationPlayerManager::instance()->create(0, m_navigator, ui->graphicsView));
    m_secondaryNavigator->setPlayer(NavigationPlayerManager::instance()->create(0, m_secondaryNavigator.data(), nullptr));

    // Init images cache
    m_imagesCache->setStrategy(new LoopImagesCacheStrategy(m_imagesCache.data(), m_navigator));

    // Main menu bar
    menuBar()->setVisible(false);
    MainMenuBarManager::Context menuBarCtx;
    menuBarCtx.menuBar = menuBar();
    menuBarCtx.primaryNavigator = m_navigator;
    menuBarCtx.secondaryNavigator = m_secondaryNavigator.data();
    menuBarCtx.pPrimaryGraphicsView = &ui->graphicsView;
    menuBarCtx.pSecondaryGraphicsView = &m_secondaryMainGraphicsView;
    menuBarCtx.navigatorSynchronizer = &m_navigatorSynchronizer;
    menuBarCtx.imagesCache = m_imagesCache;
    menuBarCtx.imageViewsParentLayout = ui->pageImageViewLayout;

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
    connect(toolBarSigMapper, &QSignalMapper::mappedInt,
            ui->stackedViews, &QStackedWidget::setCurrentIndex);

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

    // Stacked views
    ui->pageFav->layout()->setContentsMargins(0, 0, 0, 0);
    connect(ui->playListGallery, SIGNAL(mouseDoubleClicked()),
            this, SLOT(loadSelectedPlayList()));
    connect(ui->playListGallery, SIGNAL(keyEnterPressed()),
            this, SLOT(loadSelectedPlayList()));

    // Fav view
    ui->pageFav->layout()->setSpacing(15);

    ui->pageGallery->layout()->setContentsMargins(0, 0, 0, 0);
    ui->pageGallery->layout()->setSpacing(0);
    ui->sideView->hide();
    connect(ui->gallery, SIGNAL(mouseDoubleClicked()),
            m_actToolBarImage, SLOT(trigger()));
    connect(ui->gallery, SIGNAL(keyEnterPressed()),
            m_actToolBarImage, SLOT(trigger()));
    connect(m_mainGraphicsViewModel.data(), SIGNAL(mouseDoubleClicked()),
            m_actToolBarGallery, SLOT(trigger()));

    ui->pageImageView->layout()->setContentsMargins(0, 0, 0, 0);

    // Init Basket
    ui->basketPane->hide();
    ui->basketPane->layout()->setSpacing(0);
    ui->basketPane->setMaximumHeight(qApp->primaryScreen()->geometry().height() / 3);
    ui->basketView->setBasketModel(&m_basket);
    ui->gallery->setBasketModel(&m_basket);
    connect(ui->basketView, &BasketView::commit, this, &MainWindow::basketCommited);

    // Init image source manager client
    ImageSourceManager::instance()->setClient(new ImageSourceManagerClientImpl(this));

    // Init image path corrector client
    ImagePathCorrector::PromptClient *imagePathCorrectorClientImpl = new ImagePathCorrectorClientImpl(this);
    m_imagePathCorrector = new ImagePathCorrector(GlobalConfig::instance()->searchDirs(),
        m_navigator, imagePathCorrectorClientImpl);
    Logger::instance()->addListener(Logger::IMAGE_LOAD_ERROR,
        m_imagePathCorrector);
    Logger::instance()->addListener(Logger::IMAGE_THUMBNAIL_LOAD_ERROR,
        m_imagePathCorrector);

    connect(&m_playListEntitiesLoadWatcher, &QFutureWatcher<QList<QSharedPointer<PlayListEntity> > >::finished,
        this, &MainWindow::playListProviderEntitiesLoaded);
    connect(&m_playListEntityTriggerWatcher, &QFutureWatcher<QPair<PlayListEntityTriggerResult, QSharedPointer<PlayListEntity> > >::finished,
        this, &MainWindow::playListEntityTriggered);
    connect(&m_playListCreateWatcher, &QFutureWatcher<QList<QUrl>>::finished,
        this, &MainWindow::playListCreated);
    connect(&m_playListContinueWatcher, &QFutureWatcher<QList<QUrl>>::finished,
        this, &MainWindow::playListContinued);
    auto providers = PlayListProviderManager::instance()->instance()->all();
    auto providerButtonGroup = new QActionGroup(this);
    for (auto provider : providers) {
        auto act = ui->playListProvidersToolBar->addAction(provider->name(), [this, provider]() {
            this->loadSelectedPlayListProvider(provider->type());
        });
        act->setCheckable(true);
        providerButtonGroup->addAction(act);
    }
    providerButtonGroup->actions().first()->trigger();

    QSettings setting;
    QString fsRootPath = setting.value("LAST_FILE_SYSTEM_PATH", QDir::homePath()).toString();
    auto fsPlayListProvider = static_cast<FileSystemPlayListProvider *>(PlayListProviderManager::instance()->get(FileSystemPlayListProvider::TYPE));
    fsPlayListProvider->setRootPath(fsRootPath);

    connect(&m_localPlayListEntityCreateWatcher, &QFutureWatcher<QString>::finished, this, &MainWindow::localPlayListEntityCreated);
}

void MainWindow::createMainImageView(QWidget **pWidget, QWidget *parent, MainGraphicsViewModel *viewModel)
{
    QWidget *mainGraphicsViewWidget = nullptr;
#if defined(Q_OS_WIN32) && !defined(ENABLE_OPENGL)
    try {
        D2DMainGraphicsWidget *mainGraphicsView = new D2DMainGraphicsWidget(parent);
        mainGraphicsView->setModel(viewModel);
        viewModel->setView(mainGraphicsView);
        mainGraphicsViewWidget = mainGraphicsView;
    } catch (const DX::Exception &ex) {
        qDebug() << "Fail to create D2DMainGraphicsWidget due to" << ex.result;

        // Fall back if fail to initialize D2D
        MainGraphicsView *mainGraphicsView = new MainGraphicsView(parent);
        mainGraphicsView->setModel(viewModel);
        viewModel->setView(mainGraphicsView);
        mainGraphicsViewWidget = mainGraphicsView;
    }
#else
    MainGraphicsView *mainGraphicsView = new MainGraphicsView(parent);
#ifdef ENABLE_OPENGL
    auto mainGraphicsGLWidget = new QOpenGLWidget(mainGraphicsView);
    QSurfaceFormat mainGraphicsSurfaceFormat;
    mainGraphicsSurfaceFormat.setSamples(16);
    mainGraphicsGLWidget->setFormat(mainGraphicsSurfaceFormat);
    mainGraphicsView->setViewport(mainGraphicsGLWidget);
#endif
    mainGraphicsView->setModel(viewModel);
    viewModel->setView(mainGraphicsView);
    mainGraphicsViewWidget = mainGraphicsView;
#endif
    parent->layout()->addWidget(mainGraphicsViewWidget);
    *pWidget = mainGraphicsViewWidget;
}

void MainWindow::loadSelectedPlayListProvider(int type)
{
    PlayListProvider *provider = PlayListProviderManager::instance()->get(type);

    if (m_currentPlayListProvider != provider) {
        if (m_currentPlayListProvider) {
            disconnect(m_currentPlayListProvider, &PlayListProvider::entitiesChanged, this, &MainWindow::loadCurrentPlayListProvider);

            auto view = m_playListProviderViewManager.get(m_currentPlayListProvider->type());
            if (view) {
                view->tearDownToolBar(ui->playListProviderToolBar);
            }
        }

        m_currentPlayListProvider = provider;
        connect(m_currentPlayListProvider, &PlayListProvider::entitiesChanged, this, &MainWindow::loadCurrentPlayListProvider);

        auto view = m_playListProviderViewManager.get(m_currentPlayListProvider->type());
        if (view) {
            view->setUpToolBar(ui->playListProviderToolBar);
        }
    }

    loadCurrentPlayListProvider();
}

void MainWindow::loadCurrentPlayListProvider()
{
    auto provider = m_currentPlayListProvider;
    m_playListEntitiesLoadWatcher.setFuture(QtConcurrent::run([provider]() {
        auto entities = provider->loadEntities();
        QList<QSharedPointer<PlayListEntity> > sharedEntities;
        sharedEntities.reserve(entities.count());
        while (!entities.isEmpty()) {
            sharedEntities << QSharedPointer<PlayListEntity>(entities.takeFirst());
        }
        return sharedEntities;
    }));
}

void MainWindow::playListProviderEntitiesLoaded()
{
    auto entities = m_playListEntitiesLoadWatcher.result();
    ui->playListGallery->setPlayListEntities(entities);
}

void MainWindow::playListEntityTriggered()
{
    m_currentPlayListEntityStates.clear();
    for (auto& [result, entity] : m_playListEntityTriggerWatcher.future().results()) {
        switch (result) {
        case PlayListEntityTriggerResult::LoadPlayList:
        {
            QSharedPointer<PlayListEntityLoadContext> loadCtx(
                entity->createLoadContext(), PlayListEntityLoadContext::deleter);
            m_currentPlayListEntityStates.append({ entity, loadCtx });

            // Show gallery view
            m_actToolBarGallery->trigger();
        }
        case PlayListEntityTriggerResult::None:
            continue;
        default:
            Q_UNREACHABLE();
        }
    }

    auto future = QtConcurrent::mappedReduced(m_currentPlayListEntityStates, [](const auto &state) -> QList<QSharedPointer<ImageSource>> {
        auto imageUrls = state.entity->loadImageUrls(state.loadContext.data());
        QList<QSharedPointer<ImageSource>> imageSources;
        for (const auto &url : imageUrls) {
            // This can be slow so we put it at background
            auto rawImageSources = url.isLocalFile() ?
                ImageSourceManager::instance()->createMultiple(url.toLocalFile()) :
                ImageSourceManager::instance()->createMultiple(url);
            for (ImageSource *imageSource : rawImageSources) {
                imageSources << QSharedPointer<ImageSource>(imageSource);
            }
        }
        return imageSources;
    }, [](QList<QSharedPointer<ImageSource>> &a, const QList<QSharedPointer<ImageSource>> &b) -> void {
        a.append(b);
    });
    m_playListCreateWatcher.setFuture(future);
}

void MainWindow::playListCreated()
{
    auto future = m_playListCreateWatcher.future();
    if (!future.isFinished()) {
        return;
    }
    auto imageSources = future.result();

    auto playList = QSharedPointer<PlayList>::create();
    for (auto imageSource : imageSources) {
        *playList << QSharedPointer<Image>::create(imageSource);
    }
    setPrimaryNavigatorPlayList(playList);

    // TODO
	// // Make cover image of first playlist item selected
	// PlayListGalleryItem *firstItem =
	//     static_cast<PlayListGalleryItem *>(selectedItems[0]);
	// int coverIndex = firstItem->record()->coverIndex();

	// QList<GalleryItem *> items = ui->gallery->galleryItems();
	// if (coverIndex >= 0 && coverIndex < items.count()) {
	//     GalleryItem *coverImageItem = items.at(coverIndex);
	//     ui->gallery->scene()->clearSelection();

	//     // This has to be async because QGraphicsItem::setSelected() will
	//     // have no effect if item is not visible while our item will remain
	//     // hide until thumbnail loaded.
	//     coverImageItem->scheduleSelectedAfterShown();
	// }
}

void MainWindow::continuePlayList()
{
    for (auto &state : m_currentPlayListEntityStates) {
        auto &playListEntity = state.entity;
        if (!playListEntity->supportsOption(PlayListEntityOption::Continuation))
        {
            continue;
        }
        auto &playListEntityLoadContext = state.loadContext;
        m_playListContinueWatcher.setFuture(QtConcurrent::run([&playListEntity, &playListEntityLoadContext]() {
            return playListEntity->loadImageUrls(playListEntityLoadContext.data());
        }));
    }
}

void MainWindow::playListContinued()
{
    auto future = m_playListContinueWatcher.future();
    if (!future.isFinished()) {
        return;
    }
    auto imageUrls = future.result();
    m_navigator->playList()->append(QSharedPointer<PlayList>::create(imageUrls));
}

void MainWindow::loadSelectedPlayList()
{
    QList<GalleryItem *> selectedItems =
        ui->playListGallery->selectedGalleryItems();
    if (selectedItems.count() > 0) {
        auto provider = m_currentPlayListProvider;
        QList<QSharedPointer<PlayListEntity>> entities;
        for (auto selectedItem : selectedItems) {
            PlayListGalleryItem *playListItem =
                static_cast<PlayListGalleryItem *>(selectedItem);
            entities << playListItem->entity();
        }
        m_playListEntityTriggerWatcher.setFuture(QtConcurrent::mapped(entities, [provider](auto entity) {
            auto result = provider->triggerEntity(entity.data());
            return QPair<PlayListEntityTriggerResult, QSharedPointer<PlayListEntity> >{ result, entity };
        }));
    }
}

void MainWindow::processCommandLineOptions()
{
    QCommandLineParser parser;
    parser.addPositionalArgument("paths", "Directories or files", "[paths...]");

    parser.process(*QCoreApplication::instance());
    const QStringList imagePaths = parser.positionalArguments();

    QSharedPointer<PlayList> playList = QSharedPointer<PlayList>::create();
    QStringList urlPatterns = ImageSourceManager::instance()->urlPatterns();
    foreach (const QString& imagePath, imagePaths) {
        QString pattern = imagePath.left(imagePath.indexOf("://"));
        if (urlPatterns.contains(pattern)) {
            playList->addMultiplePath(QUrl(imagePath));
        } else {
            playList->addMultiplePath(imagePath);
        }
    }

    setPrimaryNavigatorPlayList(playList);
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

    QStringList images = QFileDialog::getOpenFileNames(
        this, tr("Open"), defaultDir,
        ImageSourceManager::instance()->fileDialogFilters());

    if (images.count() == 0) {
        return;
    }

    // Record dir for next open
    QFileInfo firstFileInfo(images[0]);
    defaultDir = firstFileInfo.absoluteDir().path();

    QSharedPointer<PlayList> playList = QSharedPointer<PlayList>::create(images);
    setPrimaryNavigatorPlayList(playList);
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

    QStringList images;
    QDirIterator iter(dir, QDir::Files, QDirIterator::Subdirectories);
    while (iter.hasNext()) {
        QString path = iter.next();
        if (!ImageSourceManager::instance()->isValidNameSuffix(iter.fileInfo().suffix())) {
            continue;
        }
        images << path;
    }

    QSharedPointer<PlayList> playList = QSharedPointer<PlayList>::create(images);
    setPrimaryNavigatorPlayList(playList);
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
                bool success = image->source()->copy(destPath);
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
    ImagePtr image = m_navigator->currentImage();
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
        auto playList = m_navigator->playList();
        auto coverPath = image->thumbnailPath();
        m_localPlayListEntityCreateWatcher.setFuture(QtConcurrent::run([name, coverPath, playList]() {
            // Create and persist an empty entity
            auto provider = PlayListProviderManager::instance()->get(LocalPlayListProvider::TYPE);
            auto entity = provider->createEntity(name);
            entity->setCoverImagePath(coverPath);
            provider->insertEntity(entity);

            // Persist images to database
            auto images = playList->toImageList();
            LocalDatabase::instance()->insertImages(images);

            // Add and persist images urls to the entity
            QList<QUrl> imageUrls;
            imageUrls.reserve(images.size());
            for (auto image : images) {
                imageUrls.append(image->source()->url());
            }
            entity->addImageUrls(imageUrls);

            return name;
        }));
    }
}

void MainWindow::localPlayListEntityCreated()
{
    auto name = m_localPlayListEntityCreateWatcher.result();
    statusBar()->showMessage(QStringLiteral("PlayList %1 saved!").arg(name), 2000);
}

void MainWindow::editImageUrl(QListWidgetItem *item)
{
    int index = this->ui->playListWidget->row(item);
    auto playList = this->m_navigator->playList();
    if (!playList) {
        return;
    }
    auto image = playList->at(index);
    if (!image) {
        return;
    }

    QUrl oldUrl = image->source()->url();
    bool ok;
    QString result = QInputDialog::getText(this, tr("Edit URL"), tr("URL"), QLineEdit::Normal, oldUrl.toString(), &ok);
    if (!ok) {
        return;
    }

    QUrl newUrl(result);
    if (!newUrl.isValid()) {
        return;
    }

    ImageSource *newSource = ImageSourceManager::instance()->createSingle(newUrl);
    if (!newSource) {
        return;
    }

    if (!LocalDatabase::instance()->updateImageUrl(oldUrl, newUrl)) {
        return;
    }

    ImagePtr newImage(new Image(newUrl));
    playList->replace(index, newImage);
    m_navigator->goIndex(index, true);
    item->setText(newImage->name());
}

void MainWindow::basketCommited(BasketView::CommitOption option)
{
    switch (option) {
    case BasketView::CommitOption::Append:
    {
        auto playList = m_navigator->playList();
        auto basketPlayList = m_basket.takePlayList();
        if (playList) {
            playList->append(basketPlayList);

            if (!m_currentPlayListEntityStates.isEmpty()) {
                auto images = basketPlayList->toImageUrls();
                auto &currentPlayListEntity = m_currentPlayListEntityStates.first().entity;
                QtConcurrent::run([currentPlayListEntity, images]() {
                    currentPlayListEntity->addImageUrls(images);
                });
            }
        }
        m_basket.clear();
        break;
    }
    case BasketView::CommitOption::Replace:
    {
        setPrimaryNavigatorPlayList(m_basket.takePlayList());
        m_basket.clear();
        break;
    }
    default:
        Q_UNREACHABLE();
    }
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

void MainWindow::imageLoaded(QSharedPointer<ImageData> imageData)
{
    ImagePtr image = m_navigator->currentImage();

    // Title
    updateWindowTitle();

    // Info widget
    ui->lblName->setText(image->name());
    ui->lblWidth->setText(QString::number(imageData->defaultFrame().width()));
    ui->lblHeight->setText(QString::number(imageData->defaultFrame().height()));
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
    QSharedPointer<PlayList> pl = m_navigator->playList();

    QListWidget *list = ui->playListWidget;
    for (int i = start; i < pl->count(); ++i) {
        ImagePtr image = pl->at(i);
        list->addItem(image->name());
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

    // Title
    updateWindowTitle();

    // Sidebar
    updateSidebarTitle();
}

void MainWindow::updateWindowTitle()
{
    ImagePtr image = m_navigator->currentImage();

    QString status;
    if (image) {
        if (image->isLoading()) {
            status = "[Loading]";
        } else if (image->isError()) {
            status = "[Error]";
        } else {
            status = "";
        }
    }

    QString progress;
    QTextStream(&progress) << "["
        << (m_navigator->currentIndex() + 1) << "/"
        << m_navigator->playList()->count() << "] ";

    QString title;
    QTextStream(&title) << status << progress << image->name();
    setWindowTitle(title);
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
    if (m_keyState == KEY_STATE_SHARE) {
        if (ev->key() >= Qt::Key_1 && ev->key() <= Qt::Key_9) {
            int sharerIdx = ev->key() - Qt::Key_1;
            QList<QSharedPointer<Image>> images { m_navigator->currentImage() };
            SharerManager::instance()->share(sharerIdx, images);
            m_keyState = KEY_STATE_NORMAL;
            return;
        } else {
            m_keyState = KEY_STATE_NORMAL;
        }
    }

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

                if (!m_secondaryMainGraphicsViewModel.isNull()) {
                    m_secondaryMainGraphicsViewModel->toggleFitInView();
                    m_secondaryMainGraphicsViewModel->fitInViewIfNecessary();
                }

                break;
            case Qt::Key_O: {
                promptToOpenImage();
                break;
            }
            case Qt::Key_S:
                m_keyState = KEY_STATE_SHARE;
                break;
            case Qt::Key_P:
                promptToSaveLocalPlayList();
                break;
            case Qt::Key_G:
                switchViews();
                break;
            case Qt::Key_Backslash: {
                QWidget *currentPage = ui->stackedViews->currentWidget();
                if (currentPage == ui->pageGallery) {
                    ui->sideView->isVisible() ?
                        ui->sideView->hide() :
                        ui->sideView->show();
                } else if (currentPage == ui->pageImageView) {
                    toggleSecondaryNavigator();
                }
                break;
            }
            case Qt::Key_T: {
                QDockWidget *sidebar = ui->sidebar;
                sidebar->setVisible(!sidebar->isVisible());
                break;
            }
            case Qt::Key_R:
                m_mainGraphicsViewModel->rotate(90);

                if (!m_secondaryMainGraphicsViewModel.isNull()) {
                    m_secondaryMainGraphicsViewModel->rotate(90);
                }

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

                if (!m_secondaryMainGraphicsViewModel.isNull()) {
                    m_secondaryMainGraphicsViewModel->fitGridInView(ev->text().toInt());
                }

                break;
        }

        ev->accept();
    } else if (ev->modifiers() == Qt::ShiftModifier) {
        switch (ev->key()) {
        case Qt::Key_S:
            promptToSaveImage();
            break;
        case Qt::Key_O:
            promptToOpenDir();
            break;
        }

        ev->accept();
    } else if (ev->modifiers() == Qt::AltModifier) {
        menuBar()->setVisible(!menuBar()->isVisible());
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

void MainWindow::moveCursor(Qt::Key direction)
{
    GalleryView *galleryView = nullptr;
    const int *moveOffset; // Up, Right, Down, Left
    if (ui->stackedViews->currentWidget() == ui->pageFav) {
        // Play list view
        galleryView = ui->playListGallery;
        static const int PLAY_LIST_MOVE_OFFSET[] = { 50, 1, 50, 1 };
        moveOffset = PLAY_LIST_MOVE_OFFSET;
    } else if (ui->stackedViews->currentWidget() == ui->pageGallery) {
        // Gallery view
        galleryView = ui->gallery;
        static const int GALLERY_MOVE_OFFSET[] = { 1, 1, 1, 1 };
        moveOffset = GALLERY_MOVE_OFFSET;
    } else {
        return;
    }

    QGraphicsScene *scene = galleryView->scene();
    QList<GalleryItem *> selectedItems = galleryView->selectedGalleryItems();
    QGraphicsItem *nextItem = nullptr;
    if (selectedItems.count() == 0) {
        QList<GalleryItem *> items = galleryView->galleryItems();
        if (!items.isEmpty()) {
            nextItem = items.first();
        }
    } else if (selectedItems.count() == 1) {
        GalleryItem *selectedItem = selectedItems[0];
        QSizeF size = selectedItem->boundingRect().size();
        QPointF pos = selectedItem->scenePos();

        QPointF nextPos;
        switch (direction)
        {
        case Qt::Key_Up:
            nextPos = QPointF(pos.x() + size.width() / 2, pos.y() - moveOffset[0]);
            break;
        case Qt::Key_Right:
            nextPos = QPointF(pos.x() + size.width() + moveOffset[1], pos.y() + size.height() / 2);
            break;
        case Qt::Key_Down:
            nextPos = QPointF(pos.x() + size.width() / 2, pos.y() + size.height() + moveOffset[2]);
            break;
        case Qt::Key_Left:
            nextPos = QPointF(pos.x() - moveOffset[3], pos.y() + size.height() / 2);
            break;
        default:
            return;
        }

        nextItem = scene->itemAt(nextPos, QTransform());
    }

    if (nextItem) {
        scene->clearSelection();
        nextItem->setSelected(true);
        Q_ASSERT(!scene->views().isEmpty());
        scene->views()[0]->ensureVisible(nextItem);
    }
}

void MainWindow::toggleSecondaryNavigator()
{
    bool willEnable = !m_navigatorSynchronizer.isEnabled();
    m_navigatorSynchronizer.setEnabled(willEnable);

    // Set step size of primary player
    int stepSize = willEnable ? 2 : 1;
    m_navigator->player()->setStepSize(stepSize);

    if (m_secondaryMainGraphicsViewModel.isNull()) {
        // Create view at the first time
        Q_ASSERT(willEnable);

        m_secondaryMainGraphicsViewModel.reset(new MainGraphicsViewModel(
            m_secondaryNavigator.data(), m_imageEffectManager.data(), nullptr));
        connect(m_secondaryMainGraphicsViewModel.data(), SIGNAL(mouseDoubleClicked()),
                m_actToolBarGallery, SLOT(trigger()));

        connect(m_secondaryNavigator.data(), &Navigator::navigationChange, m_secondaryMainGraphicsViewModel.data(), &MainGraphicsViewModel::reset);
        connect(m_secondaryNavigator.data(), &Navigator::paint, m_secondaryMainGraphicsViewModel.data(), &MainGraphicsViewModel::setImage);
        connect(m_secondaryNavigator.data(), &Navigator::paintThumbnail, m_secondaryMainGraphicsViewModel.data(), &MainGraphicsViewModel::setThumbnail);

        createMainImageView(&m_secondaryMainGraphicsView, ui->pageImageView, m_secondaryMainGraphicsViewModel.data());
        ui->pageImageViewLayout->insertWidget(0, m_secondaryMainGraphicsView);

        // Create player
        m_secondaryNavigator->setPlayer(NavigationPlayerManager::instance()->create(
            m_navigator->player()->className(), m_secondaryNavigator.data(), m_secondaryMainGraphicsView));

        // Trigger a force reload to do an initial paint
        m_secondaryNavigator->goIndex(m_secondaryNavigator->currentIndex(), true);
    } else {
        // Toggle view
        QWidget *secondaryGraphicsView = m_secondaryMainGraphicsViewModel->view()->widget();
        secondaryGraphicsView->setVisible(willEnable);
    }
}

void MainWindow::setPrimaryNavigatorPlayList(QSharedPointer<PlayList> playlist)
{
    QSharedPointer<PlayListEntity> entity = m_currentPlayListEntityStates.size() == 1 ?
        m_currentPlayListEntityStates.first().entity : QSharedPointer<PlayListEntity>();
    ui->gallery->setPlayListEntity(entity);

    m_navigator->setPlayList(playlist);

    // Notify to clear image source cache
    ImageSourceManager::instance()->clearCache();
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    m_mainGraphicsViewModel->fitInViewIfNecessary();

    if (!m_secondaryMainGraphicsViewModel.isNull()) {
        m_secondaryMainGraphicsViewModel->fitInViewIfNecessary();
    }
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
