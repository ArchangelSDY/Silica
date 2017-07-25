#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QActionGroup>
#include <QMainWindow>
#include <QGraphicsScene>
#include <QScopedPointer>

#include "navigation/NavigatorSynchronizer.h"
#include "Navigator.h"
#include "PlayList.h"

namespace Ui {
class MainWindow;
}

class ImagesCache;
class ImagePathCorrector;
class MainGraphicsViewModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

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

    void imageLoaded(Image *image);
    void loadSavedPlayLists();
    void loadSelectedPlayList();
    void loadOrEnterSelectedPath();
    void loadSelectedPath();

    void promptToCreatePlayListRecord(int type);

    void fsRootPathChanged();

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

    void updateSidebarTitle();
    void switchViews();
    void toggleSecondaryNavigator();

    Ui::MainWindow *ui;

    QSharedPointer<ImagesCache> m_imagesCache;

    Navigator *m_navigator;
    QScopedPointer<MainGraphicsViewModel> m_mainGraphicsViewModel;

    QScopedPointer<Navigator> m_secondaryNavigator;
    QScopedPointer<MainGraphicsViewModel> m_secondaryMainGraphicsViewModel;
    NavigatorSynchronizer m_navigatorSynchronizer;

    QScopedPointer<MainGraphicsViewModel> m_sideViewModel;

    // TODO: Separate toolbar logic out
    QToolBar *m_toolBar;
    QActionGroup *m_toolBarActs;
    QAction *m_actToolBarFS;
    QAction *m_actToolBarFav;
    QAction *m_actToolBarGallery;
    QAction *m_actToolBarImage;

    ImagePathCorrector *m_imagePathCorrector;

    enum KeyState {
        KEY_STATE_NORMAL,
        KEY_STATE_SHARE,
    };
    KeyState m_keyState;
};

#endif // MAINWINDOW_H
