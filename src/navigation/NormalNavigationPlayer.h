#ifndef NORMALNAVIGATIONPLAYER_H
#define NORMALNAVIGATIONPLAYER_H

#include "AbstractNavigationPlayer.h"

class NormalNavigationPlayer : public AbstractNavigationPlayer
{
    Q_OBJECT
    Q_CLASSINFO("Name", "Normal Player")
public:
    explicit NormalNavigationPlayer(Navigator *navigator, QWidget *view, QObject *parent = 0);

    void goNext() override;
    void goPrev() override;
    void onEnter() override;
};

#endif // NORMALNAVIGATIONPLAYER_H
