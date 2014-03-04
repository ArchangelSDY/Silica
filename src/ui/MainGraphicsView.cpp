#include <QGraphicsItem>
#include <QMenu>
#include <QSignalMapper>

#include "MainGraphicsView.h"

static const double SCALE_FACTOR = 0.05;

MainGraphicsView::MainGraphicsView(QWidget *parent) :
    QGraphicsView(parent) ,
    m_scene(new QGraphicsScene(this)) ,
    m_imageItem(new QGraphicsPixmapItem()) ,
    m_fitInView(Fit)
{
    m_scene->setBackgroundBrush(QColor("#323A44"));
    m_scene->addItem(m_imageItem);
    m_imageItem->setTransformationMode(Qt::SmoothTransformation);
    setScene(m_scene);
}

void MainGraphicsView::setNavigator(Navigator *navigator)
{
    m_navigator = navigator;
}

void MainGraphicsView::paint(Image *image)
{
    if (image) {
        QPixmap pixmap = QPixmap::fromImage(image->data());
        m_scene->setSceneRect(pixmap.rect());
        m_imageItem->setPixmap(pixmap);

        fitInViewIfNecessary();
    }
}

void MainGraphicsView::paintThumbnail(Image *image)
{
    if (image) {
        const QSize &viewSize = size();
        QPixmap rawThumbnail = QPixmap::fromImage(image->thumbnail());
        QPixmap fitThumbnail = rawThumbnail.scaled(
            viewSize, Qt::KeepAspectRatioByExpanding);

        m_scene->setSceneRect(fitThumbnail.rect());
        m_imageItem->setPixmap(fitThumbnail);

        fitInView(fitThumbnail.rect(), Qt::KeepAspectRatio);
    }
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

void MainGraphicsView::resizeEvent(QResizeEvent *)
{
    fitInViewIfNecessary();
}

void MainGraphicsView::mouseDoubleClickEvent(QMouseEvent *)
{
    emit mouseDoubleClicked();
}

void MainGraphicsView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = new QMenu(this);

    // Auto navigation speed
    int currentNavInterval = m_navigator->autoNavigationInterval();
    QMenu *autoNavMenu = menu->addMenu("Auto Speed");
    QActionGroup *autoNavGrp = new QActionGroup(menu);
    QSignalMapper *autoNavSigMap = new QSignalMapper(menu);
    connect(autoNavSigMap, SIGNAL(mapped(int)),
            m_navigator, SLOT(setAutoNavigationInterval(int)));

    QAction *fastAutoNav = autoNavMenu->addAction(
        "Fast", autoNavSigMap, SLOT(map()));
    fastAutoNav->setCheckable(true);
    fastAutoNav->setChecked(
        currentNavInterval == Navigator::FAST_AUTO_NAVIGATION_INTERVAL);
    autoNavGrp->addAction(fastAutoNav);
    autoNavSigMap->setMapping(
        fastAutoNav, Navigator::FAST_AUTO_NAVIGATION_INTERVAL);

    QAction *mediumAutoNav = autoNavMenu->addAction(
        "Medium", autoNavSigMap, SLOT(map()));
    mediumAutoNav->setCheckable(true);
    mediumAutoNav->setChecked(
        currentNavInterval == Navigator::MEDIUM_AUTO_NAVIGATION_INTERVAL);
    autoNavGrp->addAction(mediumAutoNav);
    autoNavSigMap->setMapping(
        mediumAutoNav, Navigator::MEDIUM_AUTO_NAVIGATION_INTERVAL);

    QAction *slowAutoNavi = autoNavMenu->addAction(
        "Slow", autoNavSigMap, SLOT(map()));
    slowAutoNavi->setCheckable(true);
    slowAutoNavi->setChecked(
        currentNavInterval == Navigator::SLOW_AUTO_NAVIGATION_INTERVAL);
    autoNavGrp->addAction(slowAutoNavi);
    autoNavSigMap->setMapping(
        slowAutoNavi, Navigator::SLOW_AUTO_NAVIGATION_INTERVAL);

    // Loop
    QAction *loop = menu->addAction("Loop", m_navigator, SLOT(setLoop(bool)));
    loop->setCheckable(true);
    loop->setChecked(m_navigator->isLooping());

    menu->exec(event->globalPos());
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
