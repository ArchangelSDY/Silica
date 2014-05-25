#ifndef HOTSPOTSEDITORSELECTINGSTATE_H
#define HOTSPOTSEDITORSELECTINGSTATE_H

#include "HotspotsEditorViewState.h"

class Image;

class HotspotsEditorSelectingState : public HotspotsEditorViewState
{
    Q_OBJECT
public:
    explicit HotspotsEditorSelectingState(ViewStateManager *mgr,
                                          HotspotsEditor *editor,
                                          QObject *parent = 0);

    void keyPressEvent(QKeyEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);

    void navigationChange();

protected:
    void onEntry();
    void onExit();

private:
    void setSelectingAreaSize();
    void scaleUpSelectingArea();
    void scaleDownSelectingArea();
    int selectingAreaScaleDelta();

    Image *m_curImage;
};

#endif // HOTSPOTSEDITORSELECTINGSTATE_H
