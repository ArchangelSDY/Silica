#ifndef HOTSPOTSEDITORCONFIRMINGSTATE_H
#define HOTSPOTSEDITORCONFIRMINGSTATE_H

#include "HotspotsEditorViewState.h"

class HotspotsEditorConfirmingState : public HotspotsEditorViewState
{
    Q_OBJECT
public:
    explicit HotspotsEditorConfirmingState(ViewStateManager *mgr,
                                           HotspotsEditor *editor,
                                           QObject *parent = 0);

    void keyPressEvent(QKeyEvent *event);

protected:

    void onEntry();
    void onExit();

};

#endif // HOTSPOTSEDITORCONFIRMINGSTATE_H
