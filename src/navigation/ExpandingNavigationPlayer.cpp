#include <QWidget>

#include "ExpandingNavigationPlayer.h"
#include "Navigator.h"

ExpandingNavigationPlayer::ExpandingNavigationPlayer(Navigator *navigator,
                                                     QWidget *view,
                                                     QObject *parent) :
    AbstractNavigationPlayer(navigator, parent) ,
    m_view(view)
{
}

void ExpandingNavigationPlayer::goNext()
{
    if (m_toBeFocused.empty()) {
        m_navigator->goIndexUntilSuccess(m_navigator->currentIndex() + 1, 1);

        calcFocused();
    }

    QRectF focused = m_toBeFocused.takeFirst();
    m_navigator->focusOnRect(focused);
}

void ExpandingNavigationPlayer::goPrev()
{
    if (m_toBeFocused.empty()) {
        m_navigator->goIndexUntilSuccess(m_navigator->currentIndex() - 1, -1);

        calcFocused();
    }

    QRectF focused = m_toBeFocused.takeLast();
    m_navigator->focusOnRect(focused);
}

void ExpandingNavigationPlayer::calcFocused()
{
    Image *curImage = m_navigator->currentImage();
    if (curImage) {
        QSize imageSize = curImage->size();
        QSize viewSize = m_view->size();
        QSize scaled = imageSize.scaled(viewSize,
                                        Qt::KeepAspectRatioByExpanding);

        if (scaled.width() == viewSize.width()) {
            // Split height
            qreal ratio = scaled.height() / qreal(imageSize.height());

            int moveCount = scaled.height() / viewSize.height();
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

            int moveCount = scaled.width() / viewSize.width();
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
