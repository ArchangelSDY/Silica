#ifndef EXPANDINGNAVIGATIONPLAYER_H
#define EXPANDINGNAVIGATIONPLAYER_H

#include <QRectF>

#include "AbstractNavigationPlayer.h"

class ExpandingNavigationPlayer : public AbstractNavigationPlayer
{
    Q_OBJECT
public:
    explicit ExpandingNavigationPlayer(Navigator *navigator,
                                       QWidget *view,
                                       QObject *parent = 0);

    void goNext();
    void goPrev();

    Type type() const { return AbstractNavigationPlayer::ExpandingType; }

private:
    static const float MOVE_COUNT_FACTOR;

    void calcFocused();

    QWidget *m_view;
    QList<QRectF> m_toBeFocused;
    int m_curIndex;
};

#endif // EXPANDINGNAVIGATIONPLAYER_H
