#include "HotspotsEditorViewState.h"

HotspotsEditorViewState::HotspotsEditorViewState(ViewStateManager *mgr,
                                                 HotspotsEditor *editor,
                                                 QObject *parent) :
    ViewState(mgr, parent) ,
    m_editor(editor)
{
}
