#include "MainGraphicsViewModel.h"

#include <QMenu>
#include <QScopedPointer>

#include "image/effects/ImageEffectManager.h"
#include "share/SharerManager.h"
#include "ui/RankVoteView.h"
#include "ui/RemoteWallpapersManager.h"
#include "Navigator.h"

static const double SCALE_FACTOR = 0.05;

MainGraphicsViewModel::MainGraphicsViewModel(Navigator *navigator, ImageEffectManager *imageEffectManager) :
    m_view(0) ,
    m_navigator(navigator) ,
    m_imageEffectManager(imageEffectManager) ,
    m_image(0) ,
    m_shouldRepaintThumbnailOnShown(false) ,
    m_fitInView(Fit),
    // m_hotspotsEditor(0),
    m_rankVoteView(0) ,
    m_remoteWallpapersManager(0) ,
    m_curFrameNumber(0)
{
    connect(m_navigator, SIGNAL(focusOnRect(QRectF)),
            this, SLOT(focusOnRect(QRectF)));

    m_animationTimer.setSingleShot(true);
    connect(&m_animationTimer, SIGNAL(timeout()),
            this, SLOT(paint()));
}

MainGraphicsViewModel::~MainGraphicsViewModel()
{
}

MainGraphicsViewModel::View *MainGraphicsViewModel::view() const
{
    return m_view;
}

void MainGraphicsViewModel::setView(View *view)
{
    m_view = view;
    m_rankVoteView.reset(new RankVoteView(&m_navigator, m_view->widget()));
    m_remoteWallpapersManager.reset(new RemoteWallpapersManager(m_view->widget()));
}

void MainGraphicsViewModel::resetImage(Image *image)
{
    m_image = image;
    m_focusedRect = QRectF();
    m_curFrameNumber = 0;
    m_animationTimer.stop();
}

void MainGraphicsViewModel::paint()
{
    paint(m_image, false);
}

void MainGraphicsViewModel::paint(Image *image, bool shouldFitInView)
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

        // Apply effects
        QImage frameWithEffect = m_imageEffectManager->process(image, *frame);

        m_view->setViewportRect(frameWithEffect.rect());
        m_view->setImage(frameWithEffect);

        if (shouldFitInView) {
            fitInViewIfNecessary();
        }

        if (m_image->isAnimation()) {
            scheduleAnimation();
        }
    }
}

void MainGraphicsViewModel::paintThumbnail(Image *image)
{
    if (image) {
        if (m_image != image) {
            resetImage(image);
        }

        if (m_view->isVisible()) {
            m_shouldRepaintThumbnailOnShown = false;

            QImage fitThumbnail = image->thumbnail()->scaled(
                m_view->viewSize(), Qt::KeepAspectRatioByExpanding);

            m_view->setViewportRect(fitThumbnail.rect());
            m_view->setImage(fitThumbnail);

            m_view->setFitInView(fitThumbnail.rect(), Qt::KeepAspectRatio);
        } else {
            // QStackedViews won't layout hidden views but paintThumbnail may
            // happen before showEvent. In this case, view size is incorretly
            // small. So we have to paint thumbnail after showEvent.
            m_shouldRepaintThumbnailOnShown = true;
        }
    }
}

void MainGraphicsViewModel::focusOnRect(QRectF rect)
{
    m_focusedRect = rect;
    fitInViewIfNecessary();
}

void MainGraphicsViewModel::showEvent(QShowEvent *)
{
    if (m_shouldRepaintThumbnailOnShown) {
        paintThumbnail(m_navigator->currentImage());
    }
}

void MainGraphicsViewModel::wheelEvent(QWheelEvent *ev)
{
    ev->setAccepted(false);

    if (ev->modifiers() == Qt::ControlModifier) {
        ev->accept();

        const QPoint &p = ev->angleDelta();
        if (!p.isNull()) {
            qreal m11 = m_view->transform().m11();
            qreal m22 = m_view->transform().m22();

            if (p.y() > 0) {
                m11 += SCALE_FACTOR;
                m22 += SCALE_FACTOR;
            } else {
                // TODO: Limit min scale to be fitting in view
                m11 = (m11 - SCALE_FACTOR <= 0) ? m11 : m11 - SCALE_FACTOR;
                m22 = (m22 - SCALE_FACTOR <= 0) ? m22 : m22 - SCALE_FACTOR;
            }

            m_view->setTransform(QTransform(m11, 0, 0, m22, 0, 0));
        }
    }
}

void MainGraphicsViewModel::resizeEvent(QResizeEvent *)
{
    fitInViewIfNecessary();
}


void MainGraphicsViewModel::keyPressEvent(QKeyEvent *event)
{
    event->setAccepted(false);

    //m_hotspotsEditor->keyPressEvent(event);
    //if (event->isAccepted()) {
    //    return;
    //}

    m_rankVoteView->keyPressEvent(event);
    if (event->isAccepted()) {
        return;
    }

    // Press 'Shift + B' to add current image to basket
    if (event->modifiers() & Qt::ShiftModifier && event->key() == Qt::Key_B) {
        m_navigator->basket()->append(
            m_navigator->currentImagePtr());
        event->accept();
        return;
    }

    // Press 'Shift + W' to add as remote wallpaper
    if (event->modifiers() & Qt::ShiftModifier && event->key() == Qt::Key_W) {
        m_remoteWallpapersManager->addImageToWallpapers(
            m_navigator->currentImagePtr());
        event->accept();
        return;
    }
}

void MainGraphicsViewModel::mouseMoveEvent(QMouseEvent *event)
{
    // Reject event by default
    event->setAccepted(false);

    // m_hotspotsEditor->mouseMoveEvent(event);
}

void MainGraphicsViewModel::mousePressEvent(QMouseEvent *event)
{
    // Reject event by default
    event->setAccepted(false);

    //m_hotspotsEditor->mousePressEvent(event);

    //if (!event->isAccepted()) {
    //    QGraphicsView::mousePressEvent(event);
    //}
}

void MainGraphicsViewModel::mouseDoubleClickEvent(QMouseEvent *)
{
    emit mouseDoubleClicked();
}

void MainGraphicsViewModel::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = new QMenu();

    // Share
    QMenu *shareMenu = menu->addMenu("Share");
    QStringList sharerNames = SharerManager::instance()->sharerNames();
    QSignalMapper *shareSigMap = new QSignalMapper(shareMenu);
    connect(shareSigMap, static_cast<void (QSignalMapper::*)(int)>(&QSignalMapper::mapped),
            [this](int index) {
        SharerManager::instance()->share(index, this->m_navigator->currentImage());
    });

    for (int i = 0; i < sharerNames.count(); ++i) {
        QAction *shareAct = shareMenu->addAction(sharerNames[i], shareSigMap, SLOT(map()));
        shareSigMap->setMapping(shareAct, i);
    }

    menu->exec(event->globalPos());

    delete menu;
}

void MainGraphicsViewModel::fitGridInView(int grid)
{
    qreal width = m_view->viewportRect().width() / 3;
    qreal height = m_view->viewportRect().height() / 3;

    qreal gridX = ((grid - 1) % 3) * width;
    qreal gridY = ((grid - 1) / 3) * height;

    m_view->setFitInView(QRectF(gridX, gridY, width, height), Qt::KeepAspectRatio);
    m_fitInView = Actual;
}

void MainGraphicsViewModel::fitInViewIfNecessary()
{
    const QRectF &viewportRect = m_view->viewportRect();
    const QSize &viewSize = m_view->viewSize();

    if (m_focusedRect.isNull()) {
        if (viewportRect.width() > viewSize.width() ||viewportRect.height() > viewSize.height()) {
            if (m_fitInView == Fit) {
                m_view->setFitInView(m_view->viewportRect(), Qt::KeepAspectRatio);
            } else if (m_fitInView == FitExpand) {
                m_view->setFitInView(m_view->viewportRect(), Qt::KeepAspectRatioByExpanding);
            } else {
                m_view->setTransform(QTransform());
            }
        } else {
            // No need to fit in view(expanding in this case)
            // if image is smaller than view.
            m_view->setTransform(QTransform());
        }
    } else {
        m_view->setFitInView(m_focusedRect, Qt::KeepAspectRatio);
    }
}

void MainGraphicsViewModel::toggleFitInView()
{
    // Set focused rect to null to disable focusing temporarily
    m_focusedRect = QRectF();

    m_fitInView = static_cast<MainGraphicsViewModel::FitMode>((static_cast<int>(m_fitInView) + 1) % 3);
}

void MainGraphicsViewModel::scroll(int dx, int dy)
{
    m_view->scroll(dx, dy);
}

void MainGraphicsViewModel::rotate(qreal angle)
{
    QTransform transform = m_view->transform();
    transform.rotate(angle);
    m_view->setTransform(transform);
}

void MainGraphicsViewModel::scheduleAnimation()
{
    if (m_image && m_image->isAnimation() && m_image->frameCount() > 1) {
        int duration = m_image->durations()[m_curFrameNumber];
        if (duration == 0) {
            return;
        }

        m_animationTimer.start(duration);
        m_curFrameNumber = (m_curFrameNumber + 1) % (m_image->frameCount());
    }
}
