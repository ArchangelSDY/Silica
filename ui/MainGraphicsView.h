#ifndef MAINGRAPHICSVIEW_H
#define MAINGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QWheelEvent>

class MainGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit MainGraphicsView(QWidget *parent = 0);

    void fitGridInView(int grid);
    void fitInViewIfNecessary();
    void toggleFitInView();

protected:
    virtual void wheelEvent(QWheelEvent *);

private:
    bool m_fitInView;
};

#endif // MAINGRAPHICSVIEW_H
