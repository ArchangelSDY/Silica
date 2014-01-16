#include "MainGraphicsView.h"

static const double SCALE_FACTOR = 0.05;

MainGraphicsView::MainGraphicsView(QWidget *parent) :
    QGraphicsView(parent) ,
    m_fitInView(Fit)
{
}

void MainGraphicsView::wheelEvent(QWheelEvent *ev)
{
    if (ev->modifiers() == Qt::ControlModifier) {
        ev->accept();

        const QPoint &p = ev->angleDelta();
        if (!p.isNull()) {
            qreal m11 = transform().m11();
            qreal m22 = transform().m22();

            if (p.y() > 0) {
                m11 += SCALE_FACTOR;
                m22 += SCALE_FACTOR;
            } else {
                // TODO: Limit min scale to be fitting in view
                m11 = (m11 - SCALE_FACTOR <= 0) ? m11 : m11 - SCALE_FACTOR;
                m22 = (m22 - SCALE_FACTOR <= 0) ? m22 : m22 - SCALE_FACTOR;
            }

            setTransform(QTransform(m11, 0, 0, m22, 0, 0));
        }
    } else {
        QGraphicsView::wheelEvent(ev);
    }
}

void MainGraphicsView::fitGridInView(int grid)
{
    qreal width = sceneRect().width() / 3;
    qreal height = sceneRect().height() / 3;

    qreal gridX = ((grid - 1) % 3) * width;
    qreal gridY = ((grid - 1) / 3) * height;

    fitInView(gridX, gridY, width, height, Qt::KeepAspectRatio);
    m_fitInView = Actual;
}

void MainGraphicsView::fitInViewIfNecessary()
{
    if (sceneRect().width() > width() || sceneRect().height() > height()) {
        if (m_fitInView == Fit) {
            fitInView(sceneRect(), Qt::KeepAspectRatio);
        } else if (m_fitInView == FitExpand) {
            fitInView(sceneRect(), Qt::KeepAspectRatioByExpanding);
        } else {
            resetMatrix();
        }
    } else {
        // No need to fit in view(expanding in this case)
        // if image is smaller than view.
        resetMatrix();
    }
}

void MainGraphicsView::toggleFitInView()
{
    m_fitInView = static_cast<MainGraphicsView::FitMode>((static_cast<int>(m_fitInView) + 1) % 3);
}
