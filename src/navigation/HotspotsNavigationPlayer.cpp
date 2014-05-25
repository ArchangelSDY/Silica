#include "HotspotsNavigationPlayer.h"
#include "Navigator.h"

HotspotsNavigationPlayer::HotspotsNavigationPlayer(Navigator *navigator,
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
            m_navigator->goIndexUntilSuccess(
                m_navigator->currentIndex() + 1, 1);
            m_curHotspotIndex = 0;
        } else {
            m_curHotspotIndex ++;
            ImageHotspot *hotspot = hotspots[m_curHotspotIndex];
            m_navigator->focusOnRect(hotspot->rect());
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
            m_navigator->goIndexUntilSuccess(
                m_navigator->currentIndex() - 1, -1);

            Image *nextImage = m_navigator->currentImage();
            nextImage->loadHotspots();
            m_curHotspotIndex = nextImage->hotspots().count() - 1;
        } else {
            m_curHotspotIndex --;
            ImageHotspot *hotspot = hotspots[m_curHotspotIndex];
            m_navigator->focusOnRect(hotspot->rect());
        }
    }
}
