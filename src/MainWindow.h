#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QActionGroup>
#include <QFutureWatcher>
#include <QMainWindow>
#include <QGraphicsScene>
#include <QScopedPointer>
#include <QSharedPointer>

#include "models/BasketModel.h"
#include "navigation/NavigatorSynchronizer.h"
#include "playlist/PlayListEntityLoadContext.h"
#include "playlist/PlayListEntityTriggerResult.h"
#include "ui/gamepad/GamepadController.h"
#include "ui/playlist/PlayListProviderViewManager.h"
#include "ui/BasketView.h"
#include "Navigator.h"
#include "PlayList.h"

namespace Ui {
class MainWindow;
}

class QListWidgetItem;
class ImageEffectManager;
class ImagesCache;
class ImagePathCorrector;
class GamepadController;
class MainGraphicsViewModel;
class PlayListEntity;
class PlayListProvider;

class CurrentPlayListEntityState
{
public:
    QSharedPointer<PlayListEntity> entity;
    QSharedPointer<PlayListEntityLoadContext> loadContext;

    CurrentPlayListEntityState(
        QSharedPointer<PlayListEntity> e,
        QSharedPointer<PlayListEntityLoadContext> c);
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
    friend class GamepadController;
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void navigationChange(int index);
    void updateStatus(QString message);
    void gallerySelectionChanged();
    void statusBarMessageChanged(const QString & message);

private slots:
    void playListChange();
    void playListAppend(int start);

    void loadCurrentPlayListProvider();
    void playListProviderEntitiesLoaded();
    void playListEntityTriggered();
    void playListCreated();
    void playListContinued();

    void localPlayListEntityCreated();

    void imageLoaded(QSharedPointer<ImageData> image);
    void loadSelectedPlayList();
    void loadOrEnterSelectedPath();
    void loadSelectedPath();

    void editImageUrl(QListWidgetItem *item);

    void fsRootPathChanged();

    void basketCommited(BasketView::CommitOption);

protected:
    virtual void keyPressEvent(QKeyEvent *);
    virtual void resizeEvent(QResizeEvent *);
    virtual void changeEvent(QEvent *);

private:
    void setupExtraUi();
    void createMainImageView(QWidget **pWidget, QWidget *parent, MainGraphicsViewModel *viewModel);
    void processCommandLineOptions();
    void promptToOpenImage();
    void promptToOpenDir();
    void promptToSaveImage();
    void promptToSaveLocalPlayList();
    void showAbout();

    void updateWindowTitle();
    void updateSidebarTitle();
    void switchViews();
    void moveCursor(Qt::Key direction);
    void toggleSecondaryNavigator();
    void setPrimaryNavigatorPlayList(QSharedPointer<PlayList> playlist);

    void loadSelectedPlayListProvider(int type);
    void continuePlayList();

    Ui::MainWindow *ui;

    PlayListProviderViewManager m_playListProviderViewManager;
    PlayListProvider *m_currentPlayListProvider;
    //QSharedPointer<PlayListEntity> m_currentPlayListEntity;
    //QScopedPointer<PlayListEntityLoadContext, ScopedPointerPlayListEntityLoadContextDeleter> m_currentPlayListEntityLoadContext;
    QList<CurrentPlayListEntityState> m_currentPlayListEntityStates;
    QFutureWatcher<QList<QSharedPointer<PlayListEntity>>> m_playListEntitiesLoadWatcher;
    QFutureWatcher<QPair<PlayListEntityTriggerResult, QSharedPointer<PlayListEntity> > > m_playListEntityTriggerWatcher;
    QFutureWatcher<QList<QSharedPointer<ImageSource>>> m_playListCreateWatcher;
    QFutureWatcher<QList<QUrl>> m_playListContinueWatcher;
    QFutureWatcher<QString> m_localPlayListEntityCreateWatcher;

    QSharedPointer<ImagesCache> m_imagesCache;

    Navigator *m_navigator;
    QScopedPointer<MainGraphicsViewModel> m_mainGraphicsViewModel;

    QScopedPointer<Navigator> m_secondaryNavigator;
    QScopedPointer<MainGraphicsViewModel> m_secondaryMainGraphicsViewModel;
    QWidget *m_secondaryMainGraphicsView;
    NavigatorSynchronizer m_navigatorSynchronizer;

    QScopedPointer<MainGraphicsViewModel> m_sideViewModel;

    BasketModel m_basket;

    // TODO: Separate toolbar logic out
    QToolBar *m_toolBar;
    QActionGroup *m_toolBarActs;
    QAction *m_actToolBarFS;
    QAction *m_actToolBarFav;
    QAction *m_actToolBarGallery;
    QAction *m_actToolBarImage;

    ImagePathCorrector *m_imagePathCorrector;
    QScopedPointer<ImageEffectManager> m_imageEffectManager;

    enum KeyState {
        KEY_STATE_NORMAL,
        KEY_STATE_SHARE,
    };
    KeyState m_keyState;

    QScopedPointer<GamepadController> m_gamepadController;
};

#endif // MAINWINDOW_H
