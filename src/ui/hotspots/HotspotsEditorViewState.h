#ifndef HOTSPOTSEDITORVIEWSTATE_H
#define HOTSPOTSEDITORVIEWSTATE_H

#include "ViewState.h"

class HotspotsEditor;

class HotspotsEditorViewState : public ViewState
{
    Q_OBJECT
public:
    explicit HotspotsEditorViewState(ViewStateManager *mgr,
                                     HotspotsEditor *editor,
                                     QObject *parent = 0);

protected:
    HotspotsEditor *m_editor;
};

#endif // HOTSPOTSEDITORVIEWSTATE_H
