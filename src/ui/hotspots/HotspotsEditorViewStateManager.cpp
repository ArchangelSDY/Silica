#include "HotspotsEditorViewState.h"
#include "HotspotsEditorViewStateManager.h"

HotspotsEditorViewStateManager::HotspotsEditorViewStateManager(QObject *parent) :
    ViewStateManager(parent)
{
}

void HotspotsEditorViewStateManager::navigationChange(int)
{
    HotspotsEditorViewState *state =
        static_cast<HotspotsEditorViewState *>(m_curState);
    state->navigationChange();
}
