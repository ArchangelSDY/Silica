#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QActionGroup>
#include <QMainWindow>
#include <QGraphicsScene>

#include "Navigator.h"
#include "PlayList.h"

namespace Ui {
class MainWindow;
}

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

    void promptToSaveRemotePlayList();

protected:
    virtual void keyPressEvent(QKeyEvent *);
    virtual void resizeEvent(QResizeEvent *);
    virtual void changeEvent(QEvent *);

private:
    void setupExtraUi();
    void processCommandLineOptions();
    void promptToOpenImage();
    void promptToOpenDir();
    void promptToSaveImage();
    void promptToSaveLocalPlayList();
    void showAbout();

    void updateSidebarTitle();
    void switchViews();

    Ui::MainWindow *ui;
    Navigator *m_navigator;

    // TODO: Separate toolbar logic out
    QToolBar *m_toolBar;
    QActionGroup *m_toolBarActs;
    QAction *m_actToolBarFS;
    QAction *m_actToolBarFav;
    QAction *m_actToolBarGallery;
    QAction *m_actToolBarImage;

};

#endif // MAINWINDOW_H
