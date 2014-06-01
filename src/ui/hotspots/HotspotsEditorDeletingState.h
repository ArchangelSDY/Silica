#ifndef HOTSPOTSEDITORDELETINGSTATE_H
#define HOTSPOTSEDITORDELETINGSTATE_H

#include "HotspotsEditorViewState.h"

class HotspotsEditorDeletingState : public HotspotsEditorViewState
{
    Q_OBJECT
public:
    explicit HotspotsEditorDeletingState(ViewStateManager *mgr,
                                         HotspotsEditor *editor,
                                         QObject *parent = 0);

    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);

protected:
    void onEntry();
    void onExit();
};

#endif // HOTSPOTSEDITORDELETINGSTATE_H
