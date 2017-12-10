#include "HotspotsNavigationPlayer.h"

#include "image/ImageHotspot.h"
#include "Navigator.h"

HotspotsNavigationPlayer::HotspotsNavigationPlayer(Navigator *navigator,
                                                   QWidget *view,
                                                   QObject *parent) :
    AbstractNavigationPlayer(navigator, parent) ,
    m_curHotspotIndex(0)
{
}

void HotspotsNavigationPlayer::goNext()
{
    Image *image = m_navigator->currentImage();
    if (image) {
        image->loadHotspots();
        const QList<ImageHotspot *> &hotspots = image->hotspots();

        if (m_curHotspotIndex >= hotspots.count() - 1) {
            // Already at last hotspot, should go to next image
            m_navigator->goIndexUntilSuccess(
                m_navigator->currentIndex() + 1, 1);

            // Load hotspots of next image
            Image *nextImage = m_navigator->currentImage();
            nextImage->loadHotspots();
            const QList<ImageHotspot *> &nextHotspots = nextImage->hotspots();

            // Go to first hotspot if any
            m_curHotspotIndex = 0;
            goToHotspot(nextHotspots, m_curHotspotIndex);
        } else {
            // Not at last hotspot, can go to next hostspot
            m_curHotspotIndex ++;
            goToHotspot(hotspots, m_curHotspotIndex);
        }
    }
}

void HotspotsNavigationPlayer::goPrev()
{
    Image *image = m_navigator->currentImage();
    if (image) {
        image->loadHotspots();
        const QList<ImageHotspot *> &hotspots = image->hotspots();

        if (m_curHotspotIndex <= 0) {
            // Already at first hotspot, should go to prev image
            m_navigator->goIndexUntilSuccess(
                m_navigator->currentIndex() - 1, -1);

            Image *prevImage = m_navigator->currentImage();
            prevImage->loadHotspots();
            const QList<ImageHotspot *> &prevHotspots = prevImage->hotspots();

            m_curHotspotIndex = prevHotspots.count() - 1;
            goToHotspot(prevHotspots, m_curHotspotIndex);
        } else {
            // Not at first hotspot, can go to prev hotspot
            m_curHotspotIndex --;
            goToHotspot(hotspots, m_curHotspotIndex);
        }
    }
}

void HotspotsNavigationPlayer::goToHotspot(
        const QList<ImageHotspot *> &hotspots, int index)
{
    if (index >= 0 && index < hotspots.count()) {
        m_navigator->focusOnRect(hotspots[index]->rect());
    } else {
        m_navigator->focusOnRect(QRectF());
    }
}
