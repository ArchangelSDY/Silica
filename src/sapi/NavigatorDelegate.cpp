#include "NavigatorDelegate.h"

#include "sapi/ImageDelegate.h"
#include "Navigator.h"

namespace sapi {

NavigatorDelegate::NavigatorDelegate(Navigator *navigator) :
    m_navigator(navigator)
{
}

int NavigatorDelegate::currentIndex() const
{
    return m_navigator->currentIndex();
}

QSharedPointer<sapi::ImageResource> NavigatorDelegate::currentImage()
{
    Image *currentImage = m_navigator->currentImage();
    if (currentImage) {
        return QSharedPointer<sapi::ImageResource>(new sapi::ImageDelegate(currentImage));
    } else {
        return QSharedPointer<sapi::ImageResource>();
    }
}

void NavigatorDelegate::goIndexUntilSuccess(int index, int delta)
{
    m_navigator->goIndexUntilSuccess(index, delta);
}

void NavigatorDelegate::focusOnRect(const QRectF &rect)
{
    m_navigator->focusOnRect(rect);
}

void NavigatorDelegate::repaint()
{
    m_navigator->repaint();
}

}