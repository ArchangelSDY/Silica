#include <QGraphicsItem>
#include <QMenu>
#include <QSignalMapper>

#include "Image.h"
#include "Navigator.h"
#include "MainGraphicsView.h"
#include "RankVoteView.h"
#include "RemoteWallpapersManager.h"

static const double SCALE_FACTOR = 0.05;

MainGraphicsView::MainGraphicsView(QWidget *parent) :
    QGraphicsView(parent) ,
    m_navigator(0) ,
    m_scene(new QGraphicsScene(this)) ,
    m_imageItem(new QGraphicsPixmapItem()) ,
    m_image(0),
    m_shouldRepaintThumbnailOnShown(false) ,
    m_fitInView(Fit) ,
    m_hotspotsEditor(0) ,
    m_rankVoteView(new RankVoteView(&m_navigator, this)) ,
    m_remoteWallpapersMgr(new RemoteWallpapersManager(this)) ,
    m_curFrameNumber(0)
{
    m_scene->setBackgroundBrush(QColor("#323A44"));
    m_scene->addItem(m_imageItem);
    m_imageItem->setTransformationMode(Qt::SmoothTransformation);

    setMouseTracking(true);

    setScene(m_scene);

    // Hotspots editor must be initialized after scene's added to view
    // because its mode indicator needs a parent widget
    m_hotspotsEditor = new HotspotsEditor(m_scene, &m_navigator);

    m_animationTimer.setSingleShot(true);
    connect(&m_animationTimer, SIGNAL(timeout()),
            this, SLOT(paint()));
}

MainGraphicsView::~MainGraphicsView()
{
    delete m_hotspotsEditor;
}

void MainGraphicsView::setNavigator(Navigator *navigator)
{
    m_navigator = navigator;
    connect(m_navigator, SIGNAL(navigationChange(int)),
            m_hotspotsEditor, SLOT(navigationChange(int)));
    connect(m_navigator, SIGNAL(focusOnRect(QRectF)),
            this, SLOT(focusOnRect(QRectF)));
}

void MainGraphicsView::resetImage(Image *image)
{
    m_image = image;
    m_focusedRect = QRectF();
    m_curFrameNumber = 0;
    m_animationTimer.stop();
}

void MainGraphicsView::paint()
{
    paint(m_image, false);
}

void MainGraphicsView::paint(Image *image, bool shouldFitInView)
{
    if (image) {
        if (m_image != image) {
            resetImage(image);
        }

        m_shouldRepaintThumbnailOnShown = false;

        if (m_curFrameNumber < 0 || m_curFrameNumber >= image->frameCount()) {
            m_curFrameNumber = 0;
        }

        QImage *frame = image->frames()[m_curFrameNumber];
        QPixmap pixmap = QPixmap::fromImage(*frame);
        m_scene->setSceneRect(pixmap.rect());
        m_imageItem->setPixmap(pixmap);

        if (shouldFitInView) {
            fitInViewIfNecessary();
        }

        if (m_image->isAnimation()) {
            scheduleAnimation();
        }
    }
}

void MainGraphicsView::paintThumbnail(Image *image)
{
    if (image) {
        if (m_image != image) {
            resetImage(image);
        }

        if (isVisible()) {
            m_shouldRepaintThumbnailOnShown = false;

            const QSize &viewSize = size();
            QPixmap rawThumbnail = QPixmap::fromImage(image->thumbnail());
            QPixmap fitThumbnail = rawThumbnail.scaled(
                viewSize, Qt::KeepAspectRatioByExpanding);

            m_scene->setSceneRect(fitThumbnail.rect());
            m_imageItem->setPixmap(fitThumbnail);

            fitInView(fitThumbnail.rect(), Qt::KeepAspectRatio);
        } else {
            // QStackedViews won't layout hidden views but paintThumbnail may
            // happen before showEvent. In this case, view size is incorretly
            // small. So we have to paint thumbnail after showEvent.
            m_shouldRepaintThumbnailOnShown = true;
        }
    }
}

void MainGraphicsView::focusOnRect(QRectF rect)
{
    m_focusedRect = rect;
    fitInViewIfNecessary();
}

void MainGraphicsView::showEvent(QShowEvent *)
{
    if (m_shouldRepaintThumbnailOnShown) {
        paintThumbnail(m_navigator->currentImage());
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

void MainGraphicsView::keyPressEvent(QKeyEvent *event)
{
    // Reject event by default
    event->setAccepted(false);

    m_hotspotsEditor->keyPressEvent(event);
    if (event->isAccepted()) {
        return;
    }

    m_rankVoteView->keyPressEvent(event);
    if (event->isAccepted()) {
        return;
    }

    // Press 'Shift + B' to add current image to basket
    if (event->modifiers() & Qt::ShiftModifier && event->key() == Qt::Key_B) {
        Navigator::instance()->basket()->append(
            Navigator::instance()->currentImagePtr());
        event->accept();
        return;
    }

    // Press 'Shift + W' to add as remote wallpaper
    if (event->modifiers() & Qt::ShiftModifier && event->key() == Qt::Key_W) {
        m_remoteWallpapersMgr->addImageToWallpapers(
            Navigator::instance()->currentImagePtr());
    }

    QGraphicsView::keyPressEvent(event);
}

void MainGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    // Reject event by default
    event->setAccepted(false);

    m_hotspotsEditor->mouseMoveEvent(event);

    if (!event->isAccepted()) {
        QGraphicsView::mouseMoveEvent(event);
    }
}

void MainGraphicsView::mousePressEvent(QMouseEvent *event)
{
    // Reject event by default
    event->setAccepted(false);

    m_hotspotsEditor->mousePressEvent(event);

    if (!event->isAccepted()) {
        QGraphicsView::mousePressEvent(event);
    }
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
    if (m_focusedRect.isNull()) {
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
    } else {
        fitInView(m_focusedRect, Qt::KeepAspectRatio);
    }
}

void MainGraphicsView::toggleFitInView()
{
    // Set focused rect to null to disable focusing temporarily
    m_focusedRect = QRectF();

    m_fitInView = static_cast<MainGraphicsView::FitMode>((static_cast<int>(m_fitInView) + 1) % 3);
}

void MainGraphicsView::scheduleAnimation()
{
    if (m_image && m_image->isAnimation() && m_image->frameCount() > 1) {
        int duration = m_image->durations()[m_curFrameNumber];
        m_animationTimer.start(duration);
        m_curFrameNumber = (m_curFrameNumber + 1) % (m_image->frameCount());
    }
}
