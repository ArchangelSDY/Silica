#include <QWidget>

#include "ExpandingNavigationPlayer.h"
#include "Navigator.h"

const float ExpandingNavigationPlayer::MOVE_COUNT_FACTOR = 1.5;

ExpandingNavigationPlayer::ExpandingNavigationPlayer(Navigator *navigator,
                                                     QWidget *view,
                                                     QObject *parent) :
    AbstractNavigationPlayer(navigator, parent) ,
    m_view(view) ,
    m_curIndex(0)
{
}

void ExpandingNavigationPlayer::goNext()
{
    if (m_curIndex >= m_toBeFocused.count() - 1) {
        m_navigator->goIndexUntilSuccess(m_navigator->currentIndex() + 1, 1);

        calcFocused();
        m_curIndex = 0;
    } else {
        m_curIndex ++;
    }

    QRectF focused = m_toBeFocused.at(m_curIndex);
    m_navigator->focusOnRect(focused);
}

void ExpandingNavigationPlayer::goPrev()
{
    if (m_curIndex <= 0) {
        m_navigator->goIndexUntilSuccess(m_navigator->currentIndex() - 1, -1);

        calcFocused();
        m_curIndex = m_toBeFocused.count() - 1;
    } else {
        m_curIndex --;
    }

    QRectF focused = m_toBeFocused.at(m_curIndex);
    m_navigator->focusOnRect(focused);
}

void ExpandingNavigationPlayer::calcFocused()
{
    Image *curImage = m_navigator->currentImage();
    if (curImage) {
        m_toBeFocused.clear();

        QSize imageSize = curImage->size();
        QSize viewSize = m_view->size();
        QSize scaled = imageSize.scaled(viewSize,
                                        Qt::KeepAspectRatioByExpanding);

        if (scaled.width() == viewSize.width()) {
            // Split height
            qreal ratio = scaled.height() / qreal(imageSize.height());

            int moveCount = scaled.height() / viewSize.height()
                * ExpandingNavigationPlayer::MOVE_COUNT_FACTOR;
            qreal offsetDelta =
                (scaled.height() - viewSize.height()) / moveCount;

            qreal offsetDeltaOnImage = offsetDelta / ratio;
            qreal viewHeightOnImage = viewSize.height() / ratio;

            qreal offset = 0;
            for (int i = 0; i <= moveCount; ++i) {
                QRectF focused(0, offset, imageSize.width(),
                    viewHeightOnImage);
                m_toBeFocused.append(focused);

                offset += offsetDeltaOnImage;
            }
        } else {
            // Split width
            qreal ratio = scaled.width() / qreal(imageSize.width());

            int moveCount = scaled.width() / viewSize.width() * 2
                * ExpandingNavigationPlayer::MOVE_COUNT_FACTOR;
            qreal offsetDelta =
                (scaled.width() - viewSize.width()) / moveCount;

            qreal offsetDeltaOnImage = offsetDelta / ratio;
            qreal viewWidthOnImage = viewSize.width() / ratio;

            qreal offset = 0;
            for (int i = 0; i <= moveCount; ++i) {
                QRectF focused(offset, 0, viewWidthOnImage,
                    imageSize.height());
                m_toBeFocused.append(focused);

                offset += offsetDeltaOnImage;
            }
        }
    }
}
