#ifndef MAINGRAPHICSVIEW_H
#define MAINGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QWheelEvent>

class MainGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit MainGraphicsView(QWidget *parent = 0);

protected:
    virtual void wheelEvent(QWheelEvent *);
};

#endif // MAINGRAPHICSVIEW_H