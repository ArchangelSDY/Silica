#ifndef MAINGRAPHICSVIEW_H
#define MAINGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QTimer>
#include <QWheelEvent>

#include "HotspotsEditor.h"
#include "Image.h"
#include "Navigator.h"

class Image;
class RankVoteView;
class RemoteWallpapersManager;

class MainGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    enum FitMode {
        Fit = 0,
        FitExpand = 1,
        Actual = 2,
    };

    explicit MainGraphicsView(QWidget *parent = 0);

    void setNavigator(Navigator *navigator);

    void fitGridInView(int grid);
    void fitInViewIfNecessary();
    void toggleFitInView();

    HotspotsEditor *hotspotsEditor() { return m_hotspotsEditor; }

signals:
    void mouseDoubleClicked();

private slots:
    void paint();
    void paint(Image *image, bool shouldFitInView = true);
    void paintThumbnail(Image *image);
    void focusOnRect(QRectF rect);

protected:
    virtual void showEvent(QShowEvent *event);
    virtual void wheelEvent(QWheelEvent *);
    virtual void resizeEvent(QResizeEvent *);
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseDoubleClickEvent(QMouseEvent *);
    virtual void contextMenuEvent(QContextMenuEvent *event);

private:
    void scheduleAnimation();
    void resetImage(Image *image);

    Navigator *m_navigator;
    QGraphicsScene *m_scene;
    QGraphicsPixmapItem *m_imageItem;
    Image *m_image;
    bool m_shouldRepaintThumbnailOnShown;

    FitMode m_fitInView;

    HotspotsEditor *m_hotspotsEditor;
    QRectF m_focusedRect;

    RankVoteView *m_rankVoteView;
    RemoteWallpapersManager *m_remoteWallpapersMgr;

    QTimer m_animationTimer;
    int m_curFrameNumber;
};

#endif // MAINGRAPHICSVIEW_H
