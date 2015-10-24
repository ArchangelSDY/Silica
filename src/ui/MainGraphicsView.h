#ifndef MAINGRAPHICSVIEW_H
#define MAINGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QTimer>
#include <QWheelEvent>

#include "ui/models/MainGraphicsViewModel.h"
#include "HotspotsEditor.h"

class MainGraphicsView : public QGraphicsView, public MainGraphicsViewModel::View
{
    Q_OBJECT
public:
    explicit MainGraphicsView(QWidget *parent = 0);
    virtual ~MainGraphicsView();

    //HotspotsEditor *hotspotsEditor() { return m_hotspotsEditor; }

    virtual bool isVisible() const;

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
    virtual void setImage(const QImage &image) ;
    virtual void setViewportRect(const QRect &rect);
    virtual void setTransform(const QTransform &transform);
    virtual void setFitInView(const QRectF & rect, Qt::AspectRatioMode aspectRatioMode = Qt::IgnoreAspectRatio);
    virtual QSize viewSize() const;
    virtual QRectF viewportRect() const;
    virtual QTransform transform() const;
    virtual QWidget *widget();

    QGraphicsScene *m_scene;
    QGraphicsPixmapItem *m_imageItem;
};

#endif // MAINGRAPHICSVIEW_H
