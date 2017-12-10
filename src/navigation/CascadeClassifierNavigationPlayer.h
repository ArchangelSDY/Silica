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
    Q_OBJECT
    Q_CLASSINFO("Name", "Cascade Classifier Player")
public:
    CascadeClassifierNavigationPlayer(Navigator *navigator,
                                      QWidget *view,
                                      QObject *parent = 0);

    void goPrev() override;
    void goNext() override;
    void onEnter() override;

private:
    void detectRegions();
    void focusOnCurrentRegion();

    QWidget *m_view;
    QScopedPointer<cv::CascadeClassifier> m_classifier;
    int m_curRegionIndex;
    QList<QRect> m_regions;
};

#endif // CASCADECLASSIFIERNAVIGATIONPLAYER_H
