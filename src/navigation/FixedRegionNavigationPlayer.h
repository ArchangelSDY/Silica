#ifndef FIXEDREGIONNAVIGATIONPLAYER_H
#define FIXEDREGIONNAVIGATIONPLAYER_H

#include "AbstractNavigationPlayer.h"

class FixedRegionNavigationPlayer : public AbstractNavigationPlayer
{
    Q_OBJECT
public:
    explicit FixedRegionNavigationPlayer(Navigator *navigator,
                                         QWidget *view,
                                         QObject *parent = 0);

    void goNext();
    void goPrev();

    Type type() const { return AbstractNavigationPlayer::FixedRegionType; }

    QDialog *configureDialog() const;

private:
    QRectF calcFocusedRect() const;

    QWidget *m_view;
    qreal m_centerPosition;
};

#endif // FIXEDREGIONNAVIGATIONPLAYER_H
