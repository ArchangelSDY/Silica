#include "MainGraphicsView.h"

static const double SCALE_FACTOR = 0.05;

MainGraphicsView::MainGraphicsView(QWidget *parent) :
    QGraphicsView(parent)
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
