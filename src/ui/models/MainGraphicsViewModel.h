#ifndef MAINGRAPHICSVIEWMODEL_H
#define MAINGRAPHICSVIEWMODEL_H

#include <QImage>
#include <QObject>
#include <QRectF>
#include <QScopedPointer>
#include <QSharedPointer>
#include <QTimer>

#include "image/Image.h"

class QContextMenuEvent;
class QKeyEvent;
class QMouseEvent;
class QResizeEvent;
class QShowEvent;
class QWheelEvent;

class BasketModel;
class ImageEffectManager;
class Navigator;
class RankVoteView;
class RemoteWallpapersManager;

class MainGraphicsViewModel : public QObject
{
    Q_OBJECT
public:
    enum FitMode {
        Fit = 0,
        FitExpand = 1,
        Actual = 2,
    };

    class View
    {
    public:
        virtual void setImage(const QImage &image) = 0;
        virtual void setViewportRect(const QRect &rect) = 0;;
        virtual void setTransform(const QTransform &transform) = 0;
        virtual void setFitInView(const QRectF & rect, Qt::AspectRatioMode aspectRatioMode = Qt::IgnoreAspectRatio) = 0;
        virtual void scroll(int dx, int dy) = 0;
        virtual bool isVisible() const = 0;
        virtual QSize viewSize() const = 0;
        virtual QRectF viewportRect() const = 0;
        virtual QTransform transform() const = 0;
        virtual QWidget *widget() = 0;

        void setModel(MainGraphicsViewModel *model) { m_model = model; }

    protected:
        MainGraphicsViewModel *m_model;
    };

    explicit MainGraphicsViewModel(Navigator *navigator, ImageEffectManager *imageEffectManager, BasketModel *basket);
    ~MainGraphicsViewModel();

    View *view() const;
    void setView(View *view);

    void fitGridInView(int grid);
    void fitInViewIfNecessary();
    void toggleFitInView();
    void scroll(int dx, int dy);
    void rotate(qreal angle);

    // HotspotsEditor *hotspotsEditor();

    void showEvent(QShowEvent *event);
    void wheelEvent(QWheelEvent *);
    void resizeEvent(QResizeEvent *);
    void keyPressEvent(QKeyEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *);
    void contextMenuEvent(QContextMenuEvent *event);

signals:
    void mouseDoubleClicked();

public slots:
    void reset();
    void setImage(QSharedPointer<ImageData> image);
    void setThumbnail(QSharedPointer<QImage> thumbnail);

private slots:
    void paint(bool shouldFitInView = true);
    void paintThumbnail();
    void focusOnRect(QRectF rect);

private:
    void scheduleAnimation();

    View *m_view;
    Navigator *m_navigator;
    ImageEffectManager *m_imageEffectManager;
    BasketModel *m_basket;
    QSharedPointer<ImageData> m_image;
    QSharedPointer<QImage> m_thumbnail;
    bool m_shouldRepaintThumbnailOnShown;

    FitMode m_fitInView;

    // HotspotsEditor *m_hotspotsEditor;
    QRectF m_focusedRect;

    QScopedPointer<RankVoteView> m_rankVoteView;
    QScopedPointer<RemoteWallpapersManager> m_remoteWallpapersManager;
    QTimer m_animationTimer;
    int m_curFrameNumber;
};

#endif // MAINGRAPHICSVIEWMODEL_H
