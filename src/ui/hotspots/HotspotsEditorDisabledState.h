#ifndef HOTSPOTSEDITORDISABLEDSTATE_H
#define HOTSPOTSEDITORDISABLEDSTATE_H

#include "HotspotsEditorViewState.h"

class HotspotsEditorDisabledState : public HotspotsEditorViewState
{
    Q_OBJECT
public:
    explicit HotspotsEditorDisabledState(ViewStateManager *mgr,
                                         HotspotsEditor *editor,
                                         QObject *parent = 0);

    void keyPressEvent(QKeyEvent *event);

protected:

    void onEntry();
};

#endif // HOTSPOTSEDITORDISABLEDSTATE_H
