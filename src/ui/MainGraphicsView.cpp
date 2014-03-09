#include <QGraphicsItem>
#include <QMenu>
#include <QSignalMapper>

#include "MainGraphicsView.h"

static const double SCALE_FACTOR = 0.05;

MainGraphicsView::MainGraphicsView(QWidget *parent) :
    QGraphicsView(parent) ,
    m_scene(new QGraphicsScene(this)) ,
    m_imageItem(new QGraphicsPixmapItem()) ,
    m_shouldRepaintThumbnailOnShown(false) ,
    m_fitInView(Fit) ,
    m_isHotspotsEditing(false) ,
    m_hotspotsSelectingArea(new QGraphicsRectItem()) ,
    m_hotspotsAreas(0)
{
    m_scene->setBackgroundBrush(QColor("#323A44"));
    m_scene->addItem(m_imageItem);
    m_imageItem->setTransformationMode(Qt::SmoothTransformation);

    // TODO: remove magic
    setMouseTracking(true);
    QSizeF defaultHotspotSize = qApp->primaryScreen()->size() / 3;
    m_hotspotsSelectingArea->setRect(QRectF(QPointF(), defaultHotspotSize));
    m_hotspotsSelectingArea->setBrush(QColor("#AA0000AA"));
    m_hotspotsSelectingArea->hide();
    m_scene->addItem(m_hotspotsSelectingArea);

    setScene(m_scene);
}

void MainGraphicsView::setNavigator(Navigator *navigator)
{
    m_navigator = navigator;
}

void MainGraphicsView::paint(Image *image)
{
    if (image) {
        m_shouldRepaintThumbnailOnShown = false;

        QPixmap pixmap = QPixmap::fromImage(image->data());
        m_scene->setSceneRect(pixmap.rect());
        m_imageItem->setPixmap(pixmap);

        fitInViewIfNecessary();
    }
}

void MainGraphicsView::paintThumbnail(Image *image)
{
    if (image) {
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
    if (m_isHotspotsEditing) {
        if (event->key() == Qt::Key_Escape) {
            leaveHotspotsConfirming();
            event->accept();
            return;
        } else if (event->key() == Qt::Key_Return) {
            // Save area
            saveHotspot();
            leaveHotspotsConfirming();
            event->accept();
            return;
        }
    }

    QGraphicsView::keyPressEvent(event);
}

void MainGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isHotspotsEditing && m_hotspotsEditingState == HotspotsSelecting) {
        QPointF pos = mapToScene(event->pos());
        setHotspotsSelectingAreaPos(pos);
    }
}

void MainGraphicsView::mousePressEvent(QMouseEvent *)
{
    if (m_isHotspotsEditing && m_hotspotsEditingState == HotspotsSelecting) {
        enterHotspotsConfirming();
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

void MainGraphicsView::createHotspotsAreas()
{
    if (m_hotspotsAreas) {
        m_scene->destroyItemGroup(m_hotspotsAreas);
    }

    QList<ImageHotspot *> hotspots = m_navigator->currentImage()->hotspots();
    QList<QGraphicsItem *> hotspotsAreas;
    foreach (ImageHotspot *hotspot, hotspots) {
        QGraphicsRectItem *item = new QGraphicsRectItem(hotspot->rect());
        item->setBrush(QColor("#AA0000AA"));    // TODO: magic
        hotspotsAreas << item;
    }
    m_hotspotsAreas = m_scene->createItemGroup(hotspotsAreas);
}

void MainGraphicsView::setHotspotsSelectingAreaPos(const QPointF &pos)
{
    QRectF rect = m_hotspotsSelectingArea->rect();
    QRectF sceneRect = m_scene->sceneRect();

    rect.moveTo(pos - m_hotspotsSelectingArea->rect().center());

    if (rect.left() < 0) {
        rect.setLeft(0);
    }
    if (rect.right() > sceneRect.right()) {
        rect.moveRight(sceneRect.width());
    }
    if (rect.top() < 0) {
        rect.setTop(0);
    }
    if (rect.bottom() > sceneRect.bottom()) {
        rect.moveBottom(sceneRect.bottom());
    }

    m_hotspotsSelectingArea->setPos(rect.topLeft());
}

void MainGraphicsView::enterHotspotsEditing()
{
    m_isHotspotsEditing = true;
    m_hotspotsEditingState = HotspotsSelecting;

    connect(m_navigator->currentImage(), SIGNAL(hotpotsLoaded()),
            this, SLOT(createHotspotsAreas()),
            static_cast<Qt::ConnectionType>(Qt::AutoConnection | Qt::UniqueConnection));
    m_navigator->currentImage()->loadHotspots();

    m_hotspotsSelectingArea->show();
}

void MainGraphicsView::leaveHotspotsEditing()
{
    m_hotspotsSelectingArea->hide();

    m_scene->destroyItemGroup(m_hotspotsAreas);
    m_hotspotsAreas = 0;

    disconnect(m_navigator->currentImage(), SIGNAL(hotpotsLoaded()),
               this, SLOT(createHotspotsAreas()));
}

void MainGraphicsView::enterHotspotsConfirming()
{
    m_hotspotsEditingState = HotspotsConfirming;
    m_hotspotsSelectingArea->setBrush(QColor("#AAAA0000")); // TODO: magic
}

void MainGraphicsView::leaveHotspotsConfirming()
{
    m_hotspotsSelectingArea->setBrush(QColor("#AA0000AA")); // TODO: magic
    m_hotspotsEditingState = HotspotsSelecting;
}

void MainGraphicsView::saveHotspot()
{
    ImageHotspot hotspot(m_navigator->currentImage(),
                         m_hotspotsSelectingArea->rect().toAlignedRect());
    hotspot.save();
}
