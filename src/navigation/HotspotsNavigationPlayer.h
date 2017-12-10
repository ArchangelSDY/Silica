#ifndef HOTSPOTSNAVIGATIONPLAYER_H
#define HOTSPOTSNAVIGATIONPLAYER_H

#include "AbstractNavigationPlayer.h"

class ImageHotspot;
class Navigator;

class HotspotsNavigationPlayer : public AbstractNavigationPlayer
{
    Q_OBJECT
    Q_CLASSINFO("Name", "Hotspots Player")
public:
    explicit HotspotsNavigationPlayer(Navigator *navigator,
                                      QWidget *view,
                                      QObject *parent = 0);

    void goNext() override;
    void goPrev() override;

private:
    void goToHotspot(const QList<ImageHotspot *> &hotspots, int index);

    int m_curHotspotIndex;
};

#endif // HOTSPOTSNAVIGATIONPLAYER_H
