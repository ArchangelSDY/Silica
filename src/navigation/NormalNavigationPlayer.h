#ifndef NORMALNAVIGATIONPLAYER_H
#define NORMALNAVIGATIONPLAYER_H

#include "AbstractNavigationPlayer.h"

class NormalNavigationPlayer : public AbstractNavigationPlayer
{
    Q_OBJECT
public:
    explicit NormalNavigationPlayer(Navigator *navigator, QObject *parent = 0);

    void goNext();
    void goPrev();

};

#endif // NORMALNAVIGATIONPLAYER_H
