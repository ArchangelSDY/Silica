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

    QString name() const override;
    void goNext() override;
    void goPrev() override;

private:
    void goToHotspot(const QList<ImageHotspot *> &hotspots, int index);

    int m_curHotspotIndex;
};

#endif // HOTSPOTSNAVIGATIONPLAYER_H
