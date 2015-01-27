#include <QWidget>

#include "FixedRegionNavigationPlayer.h"
#include "Navigator.h"

FixedRegionNavigationPlayer::FixedRegionNavigationPlayer(Navigator *navigator,
                                                         QWidget *view,
                                                         QObject *parent) :
    AbstractNavigationPlayer(navigator, parent) ,
    m_view(view) ,
    m_centerPosition(0.25)
{
    // Set focused rect to null to disable focusing
    m_navigator->focusOnRect(QRectF());
}

void FixedRegionNavigationPlayer::goNext()
{
    m_navigator->goIndexUntilSuccess(m_navigator->currentIndex() + 1, 1);
    m_navigator->focusOnRect(calcFocusedRect());
}

void FixedRegionNavigationPlayer::goPrev()
{
    m_navigator->goIndexUntilSuccess(m_navigator->currentIndex() - 1, -1);
    m_navigator->focusOnRect(calcFocusedRect());
}

QRectF FixedRegionNavigationPlayer::calcFocusedRect() const
{
    Image *curImage = m_navigator->currentImage();
    if (!curImage) {
        return QRectF();
    }

    QSize imageSize = curImage->size();
    QSize scaledViewSize = m_view->size().scaled(imageSize,
                                                 Qt::KeepAspectRatio);

    if (scaledViewSize.width() == imageSize.width()) {
        // Vertical
        qreal centerY = imageSize.height() * m_centerPosition;
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
        qreal centerX = imageSize.width() * m_centerPosition;
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

QDialog *FixedRegionNavigationPlayer::configureDialog() const
{
    return 0;
}

