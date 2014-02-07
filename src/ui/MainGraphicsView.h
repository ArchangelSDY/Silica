#ifndef MAINGRAPHICSVIEW_H
#define MAINGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QWheelEvent>

#include "Image.h"

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

private slots:
    void paint(Image *image);
    void paintThumbnail(Image *image);

protected:
    virtual void wheelEvent(QWheelEvent *);
    virtual void resizeEvent(QResizeEvent *);

private:
    QGraphicsScene *m_imageScene;

    FitMode m_fitInView;
};

#endif // MAINGRAPHICSVIEW_H
