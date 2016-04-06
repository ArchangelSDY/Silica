#ifndef CASCADECLASSIFIERNAVIGATIONPLAYER_H
#define CASCADECLASSIFIERNAVIGATIONPLAYER_H

#include <QList>
#include <QScopedPointer>

#include "AbstractNavigationPlayer.h"

namespace cv {
    class CascadeClassifier;
}

class CascadeClassifierNavigationPlayer : public AbstractNavigationPlayer
{
public:
    CascadeClassifierNavigationPlayer(Navigator *navigator,
                                      QWidget *view,
                                      QObject *parent = 0);

    virtual void goPrev();
    virtual void goNext();

    Type type() const { return AbstractNavigationPlayer::CascadeClassifierType; }

private:
    void detectRegions();
    void focusOnCurrentRegion();

    QWidget *m_view;
    QScopedPointer<cv::CascadeClassifier> m_classifier;
    int m_curRegionIndex;
    QList<QRect> m_regions;
};

#endif // CASCADECLASSIFIERNAVIGATIONPLAYER_H
