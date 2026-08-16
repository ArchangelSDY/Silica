#include "MainGraphicsView.h"

#include <QDebug>
#include <QGraphicsItem>
#include <QPixmap>
#include <QScrollBar>
#include <QUuid>

#include "ui/models/MainGraphicsViewModel.h"

//#include "RankVoteView.h"
//#include "RemoteWallpapersManager.h"

MainGraphicsView::MainGraphicsView(QWidget *parent) :
    QGraphicsView(parent) ,
    m_scene(new QGraphicsScene(this)) ,
    m_imageItem(new QGraphicsPixmapItem()) ,
    m_selectionItem(new QGraphicsRectItem()) ,
    m_isSelecting(false)
{
    m_scene->setBackgroundBrush(QColor("#323A44"));
    m_scene->addItem(m_imageItem);
    m_scene->addItem(m_selectionItem);
    m_imageItem->setTransformationMode(Qt::SmoothTransformation);
    m_selectionItem->setBrush(QColor(0, 120, 215, 80));
    m_selectionItem->setPen(QPen(QColor(0, 120, 215)));
    m_selectionItem->setZValue(1);
    m_selectionItem->hide();

    setMouseTracking(true);
    setDragMode(QGraphicsView::ScrollHandDrag);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setScene(m_scene);
}

MainGraphicsView::~MainGraphicsView()
{
}

void MainGraphicsView::setImage(const QImage &image)
{
    QPixmap pixmap = QPixmap::fromImage(image);
    m_scene->setSceneRect(pixmap.rect());
    m_imageItem->setPixmap(pixmap);
}

void MainGraphicsView::setViewportRect(const QRect &rect)
{
    m_scene->setSceneRect(rect);
}

void MainGraphicsView::setTransform(const QTransform &transform)
{
    QGraphicsView::setTransform(transform);
}

void MainGraphicsView::setFitInView(const QRectF &rect, Qt::AspectRatioMode aspectRatioMode)
{
    QGraphicsView::fitInView(rect, aspectRatioMode);
}

void MainGraphicsView::scroll(int dx, int dy)
{
    horizontalScrollBar()->setValue(horizontalScrollBar()->value() - dx);
    verticalScrollBar()->setValue(verticalScrollBar()->value() - dy);
}

bool MainGraphicsView::isVisible() const
{
    return QGraphicsView::isVisible();
}

QSize MainGraphicsView::viewSize() const
{
    return QGraphicsView::size();
}

QRectF MainGraphicsView::viewportRect() const
{
    return QGraphicsView::sceneRect();
}

QTransform MainGraphicsView::transform() const
{
    return QGraphicsView::transform();
}

QWidget *MainGraphicsView::widget()
{
    return this;
}

void MainGraphicsView::showEvent(QShowEvent *ev)
{
    m_model->showEvent(ev);
}

void MainGraphicsView::wheelEvent(QWheelEvent *ev)
{
    m_model->wheelEvent(ev);

    if (!ev->isAccepted()) {
        QGraphicsView::wheelEvent(ev);
    }
}

void MainGraphicsView::resizeEvent(QResizeEvent *ev)
{
    m_model->resizeEvent(ev);
}

void MainGraphicsView::keyPressEvent(QKeyEvent *ev)
{
    if (ev->key() == Qt::Key_Alt) {
        setDragMode(QGraphicsView::NoDrag);
        ev->accept();
        return;
    }

    m_model->keyPressEvent(ev);
    if (ev->isAccepted()) {
        return;
    }

    QGraphicsView::keyPressEvent(ev);
}

void MainGraphicsView::keyReleaseEvent(QKeyEvent *ev)
{
    if (ev->key() == Qt::Key_Alt) {
        setDragMode(QGraphicsView::ScrollHandDrag);
        ev->accept();
        return;
    }

    QGraphicsView::keyReleaseEvent(ev);
}

void MainGraphicsView::mouseMoveEvent(QMouseEvent *ev)
{
    if (m_isSelecting) {
        const QRectF selectionRect = QRectF(m_selectionStart, mapToScene(ev->pos()))
                                         .normalized()
                                         .intersected(sceneRect());
        m_selectionItem->setRect(selectionRect);
        ev->accept();
        return;
    }

    m_model->mouseMoveEvent(ev);
    if (ev->isAccepted()) {
        return;
    }

    QGraphicsView::mouseMoveEvent(ev);
}

void MainGraphicsView::mousePressEvent(QMouseEvent *ev)
{
    if (dragMode() == QGraphicsView::NoDrag && ev->button() == Qt::LeftButton) {
        m_selectionStart = mapToScene(ev->pos());
        m_selectionItem->setRect(QRectF(m_selectionStart, m_selectionStart));
        m_selectionItem->show();
        m_isSelecting = true;
        ev->accept();
        return;
    }

    m_model->mousePressEvent(ev);
    if (ev->isAccepted()) {
        return;
    }

    QGraphicsView::mousePressEvent(ev);
}

void MainGraphicsView::mouseReleaseEvent(QMouseEvent *ev)
{
    if (m_isSelecting && ev->button() == Qt::LeftButton) {
        m_isSelecting = false;

        if (!m_selectionItem->rect().isEmpty()) {
            qDebug() << "Selected region:" << m_selectionItem->rect();

            const QRect selectedPixels = m_imageItem->mapRectFromScene(m_selectionItem->rect())
                                             .toAlignedRect()
                                             .intersected(m_imageItem->pixmap().rect());
            if (!selectedPixels.isEmpty()) {
                const QString filePath = QStringLiteral("G:/%1.png").arg(
                    QUuid::createUuid().toString(QUuid::WithoutBraces));
                if (m_imageItem->pixmap().copy(selectedPixels).save(filePath)) {
                    qDebug() << "Saved selected region to:" << filePath;
                } else {
                    qWarning() << "Failed to save selected region to:" << filePath;
                }
            }
        }
        m_selectionItem->hide();

        ev->accept();
        return;
    }

    QGraphicsView::mouseReleaseEvent(ev);
}

void MainGraphicsView::mouseDoubleClickEvent(QMouseEvent *ev)
{
    m_model->mouseDoubleClickEvent(ev);
}

void MainGraphicsView::contextMenuEvent(QContextMenuEvent *ev)
{
    m_model->contextMenuEvent(ev);
}
