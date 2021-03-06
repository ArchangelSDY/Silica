#include <QWidget>

#include "FixedRegionConfDialog.h"
#include "FixedRegionNavigationPlayer.h"
#include "Navigator.h"

FixedRegionNavigationPlayer::FixedRegionNavigationPlayer(Navigator *navigator,
                                                         QWidget *view,
                                                         QObject *parent) :
    AbstractNavigationPlayer(navigator, parent) ,
    m_view(view) ,
    m_curCenterIndex(0)
{
}

void FixedRegionNavigationPlayer::onEnter()
{
    // Set focused rect to null to disable focusing
    m_navigator->focusOnRect(QRectF());

    // Predefined center positions
    m_centers << 0.2 << 0.5;
}

void FixedRegionNavigationPlayer::goNext()
{
    if (m_curCenterIndex >= m_centers.count() - 1) {
        m_navigator->goIndexUntilSuccess(m_navigator->currentIndex() + 1, 1);
        m_curCenterIndex = 0;
    } else {
        m_curCenterIndex ++;
    }
    m_navigator->focusOnRect(calcFocusedRect());
}

void FixedRegionNavigationPlayer::goPrev()
{
    if (m_curCenterIndex <= 0) {
        m_navigator->goIndexUntilSuccess(m_navigator->currentIndex() - 1, -1);
        m_curCenterIndex = m_centers.count() - 1;
    } else {
        m_curCenterIndex --;
    }
    m_navigator->focusOnRect(calcFocusedRect());
}

void FixedRegionNavigationPlayer::goIndexUntilSuccess(int index, int delta)
{
    m_navigator->goIndexUntilSuccess(index, delta);
    m_navigator->focusOnRect(calcFocusedRect());
}

QRectF FixedRegionNavigationPlayer::calcFocusedRect() const
{
    ImagePtr curImage = m_navigator->currentImage();
    if (!curImage || !m_view) {
        return QRectF();
    }

    qreal center = m_centers.at(m_curCenterIndex);
    QSize imageSize = curImage->size();
    QSize scaledViewSize = m_view->size().scaled(imageSize,
                                                 Qt::KeepAspectRatio);

    if (scaledViewSize.width() == imageSize.width()) {
        // Vertical
        qreal centerY = imageSize.height() * center;
        qreal topLeftY = centerY - scaledViewSize.height() / 2;
        topLeftY = topLeftY >= 0 ? topLeftY : 0;
        QPointF topLeft(0, topLeftY);

        qreal focusHeight = scaledViewSize.height();
        if (topLeftY + focusHeight > imageSize.height()) {
            focusHeight = imageSize.height() - topLeftY;
        }

        QSizeF focusSize(imageSize.width(), focusHeight);
        return QRectF(topLeft, focusSize);
    } else {
        // Horizontal
        qreal centerX = imageSize.width() * center;
        qreal topLeftX = centerX - scaledViewSize.width() / 2;
        topLeftX = topLeftX >= 0 ? topLeftX : 0;
        QPointF topLeft(topLeftX, 0);

        qreal focusWidth = scaledViewSize.width();
        if (topLeftX + focusWidth > imageSize.width()) {
            focusWidth = scaledViewSize.width() - topLeftX;
        }

        QSizeF focusSize(focusWidth, imageSize.height());
        return QRectF(topLeft, focusSize);
    }
}

QDialog *FixedRegionNavigationPlayer::createConfigureDialog()
{
    return new FixedRegionConfDialog(m_centers, m_view);
}

bool FixedRegionNavigationPlayer::isConfigurable() const
{
    return true;
}

void FixedRegionNavigationPlayer::cloneConfigurationFrom(AbstractNavigationPlayer *player)
{
    FixedRegionNavigationPlayer *other = static_cast<FixedRegionNavigationPlayer *>(player);
    m_centers = other->m_centers;
}

