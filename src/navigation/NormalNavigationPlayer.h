#ifndef NORMALNAVIGATIONPLAYER_H
#define NORMALNAVIGATIONPLAYER_H

#include "AbstractNavigationPlayer.h"

class NormalNavigationPlayer : public AbstractNavigationPlayer
{
    Q_OBJECT
public:
    explicit NormalNavigationPlayer(Navigator *navigator, QObject *parent = 0);

    QString name() const override;
    void goNext() override;
    void goPrev() override;
    void onEnter() override;

};

#endif // NORMALNAVIGATIONPLAYER_H
