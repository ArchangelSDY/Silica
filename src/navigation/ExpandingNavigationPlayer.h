#ifndef EXPANDINGNAVIGATIONPLAYER_H
#define EXPANDINGNAVIGATIONPLAYER_H

#include <QRectF>

#include "AbstractNavigationPlayer.h"

class Image;

class ExpandingNavigationPlayer : public AbstractNavigationPlayer
{
    Q_OBJECT
public:
    explicit ExpandingNavigationPlayer(Navigator *navigator,
                                       QWidget *view,
                                       QObject *parent = 0);

    void goNext();
    void goPrev();
    void reset();

    Type type() const { return AbstractNavigationPlayer::ExpandingType; }

private slots:
    void onImageSizeGet(Image *image);

private:
    static const float MOVE_COUNT_FACTOR;

    void calcFocused();

    QWidget *m_view;
    QList<QRectF> m_toBeFocused;
    int m_curIndex;
    bool m_focusAfterImageSizeGet;
};

#endif // EXPANDINGNAVIGATIONPLAYER_H
