#ifndef MAINGRAPHICSVIEW_H
#define MAINGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QWheelEvent>

#include "HotspotsEditor.h"
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

    HotspotsEditor *hotspotsEditor() { return m_hotspotsEditor; }

signals:
    void mouseDoubleClicked();

private slots:
    void paint(Image *image);
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
    Navigator *m_navigator;
    QGraphicsScene *m_scene;
    QGraphicsPixmapItem *m_imageItem;
    bool m_shouldRepaintThumbnailOnShown;

    FitMode m_fitInView;

    HotspotsEditor *m_hotspotsEditor;
};

#endif // MAINGRAPHICSVIEW_H
