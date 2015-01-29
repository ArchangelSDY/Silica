#ifndef FIXEDREGIONNAVIGATIONPLAYER_H
#define FIXEDREGIONNAVIGATIONPLAYER_H

#include <QList>

#include "AbstractNavigationPlayer.h"

class FixedRegionConfDialog;

class FixedRegionNavigationPlayer : public AbstractNavigationPlayer
{
    Q_OBJECT
public:
    explicit FixedRegionNavigationPlayer(Navigator *navigator,
                                         QWidget *view,
                                         QObject *parent = 0);
    ~FixedRegionNavigationPlayer();

    void goNext();
    void goPrev();

    Type type() const { return AbstractNavigationPlayer::FixedRegionType; }

    QDialog *configureDialog() const;

private:
    QRectF calcFocusedRect() const;

    QWidget *m_view;
    QList<qreal> m_centers;
    int m_curCenterIndex;
    FixedRegionConfDialog *m_confDialog;
};

#endif // FIXEDREGIONNAVIGATIONPLAYER_H
