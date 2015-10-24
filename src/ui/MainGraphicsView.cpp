#include "MainGraphicsView.h"

#include <QGraphicsItem>
#include <QPixmap>

#include "ui/models/MainGraphicsViewModel.h"

//#include "RankVoteView.h"
//#include "RemoteWallpapersManager.h"

MainGraphicsView::MainGraphicsView(QWidget *parent) :
    QGraphicsView(parent) ,
    m_scene(new QGraphicsScene(this)) ,
    m_imageItem(new QGraphicsPixmapItem())
{
    m_scene->setBackgroundBrush(QColor("#323A44"));
    m_scene->addItem(m_imageItem);
    m_imageItem->setTransformationMode(Qt::SmoothTransformation);

    setMouseTracking(true);

    setScene(m_scene);

    // Hotspots editor must be initialized after scene's added to view
    // because its mode indicator needs a parent widget
    // m_hotspotsEditor = new HotspotsEditor(m_scene, &m_navigator);
}

MainGraphicsView::~MainGraphicsView()
{
    // delete m_hotspotsEditor;
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
    m_model->keyPressEvent(ev);

    if (!ev->isAccepted()) {
        QGraphicsView::keyPressEvent(ev);
    }
}

void MainGraphicsView::mouseMoveEvent(QMouseEvent *ev)
{
    m_model->mouseMoveEvent(ev);

    if (!ev->isAccepted()) {
        QGraphicsView::mouseMoveEvent(ev);
    }
}

void MainGraphicsView::mousePressEvent(QMouseEvent *ev)
{
    m_model->mousePressEvent(ev);

    if (!ev->isAccepted()) {
        QGraphicsView::mousePressEvent(ev);
    }
}

void MainGraphicsView::mouseDoubleClickEvent(QMouseEvent *ev)
{
    m_model->mouseDoubleClickEvent(ev);
}

void MainGraphicsView::contextMenuEvent(QContextMenuEvent *ev)
{
    m_model->contextMenuEvent(ev);
}
