#ifndef HOTSPOTSNAVIGATIONPLAYER_H
#define HOTSPOTSNAVIGATIONPLAYER_H

#include "AbstractNavigationPlayer.h"

class ImageHotspot;
class Navigator;

class HotspotsNavigationPlayer : public AbstractNavigationPlayer
{
    Q_OBJECT
public:
    explicit HotspotsNavigationPlayer(Navigator *navigator,
                                      QObject *parent = 0);

    void goNext();
    void goPrev();

    Type type() const { return AbstractNavigationPlayer::HotspotsType; }

private:
    void goToHotspot(const QList<ImageHotspot *> &hotspots, int index);

    int m_curHotspotIndex;
};

#endif // HOTSPOTSNAVIGATIONPLAYER_H
