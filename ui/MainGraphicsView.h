#ifndef MAINGRAPHICSVIEW_H
#define MAINGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QWheelEvent>

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

    void fitGridInView(int grid);
    void fitInViewIfNecessary();
    void toggleFitInView();

protected:
    virtual void wheelEvent(QWheelEvent *);

private:
    FitMode m_fitInView;
};

#endif // MAINGRAPHICSVIEW_H
