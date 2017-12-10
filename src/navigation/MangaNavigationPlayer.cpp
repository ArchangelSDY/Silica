#include "MangaNavigationPlayer.h"

#include <QWidget>

#include "../Navigator.h"

MangaNavigationPlayer::MangaNavigationPlayer(Navigator *navigator,
                                             QWidget *view,
                                             QObject *parent) :
    AbstractNavigationPlayer(navigator, parent) ,
    m_view(view)
{
}

void MangaNavigationPlayer::goNext()
{
    m_navigator->goIndexUntilSuccess(m_navigator->currentIndex() + m_stepSize, 1);
    focus(FocusPolicy::TopRight);
}

void MangaNavigationPlayer::goPrev()
{
    m_navigator->goIndexUntilSuccess(m_navigator->currentIndex() - m_stepSize, -1);
    focus(FocusPolicy::BottomLeft);
}

void MangaNavigationPlayer::goIndexUntilSuccess(int index, int delta)
{
    m_navigator->goIndexUntilSuccess(index, delta);
    focus(delta > 0 ? FocusPolicy::TopRight : FocusPolicy::BottomLeft);
}

void MangaNavigationPlayer::onEnter()
{
    focus(FocusPolicy::TopRight);
}

void MangaNavigationPlayer::onLeave()
{
    // Set focused rect to null to disable focusing
    m_navigator->focusOnRect(QRectF());
}

void MangaNavigationPlayer::focus(FocusPolicy policy)
{
    Image *curImage = m_navigator->currentImage();
    if (!curImage || !m_view) {
        return;
    }

    QSize imageSize = curImage->size();
    QSize viewSize = m_view->size();
    QSize viewSizeScaled = viewSize.scaled(imageSize, Qt::KeepAspectRatio);

    QRectF focusRect(QPointF(), viewSizeScaled);
    switch (policy) {
    case FocusPolicy::TopRight:
        focusRect.moveTopRight(QPointF(imageSize.width(), 0));
        break;
    case FocusPolicy::BottomLeft:
        focusRect.moveBottomLeft(QPointF(0, imageSize.height()));
        break;
    default:
        return;
    }

    m_navigator->focusOnRect(focusRect);
}