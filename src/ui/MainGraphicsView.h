#ifndef MAINGRAPHICSVIEW_H
#define MAINGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QWheelEvent>

#include "Image.h"
#include "Navigator.h"

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

signals:
    void mouseDoubleClicked();

private slots:
    void paint(Image *image);
    void paintThumbnail(Image *image);

protected:
    virtual void wheelEvent(QWheelEvent *);
    virtual void resizeEvent(QResizeEvent *);
    virtual void mouseDoubleClickEvent(QMouseEvent *);
    virtual void contextMenuEvent(QContextMenuEvent *event);

private:
    Navigator *m_navigator;
    QGraphicsScene *m_scene;
    QGraphicsPixmapItem *m_imageItem;

    FitMode m_fitInView;
};

#endif // MAINGRAPHICSVIEW_H
