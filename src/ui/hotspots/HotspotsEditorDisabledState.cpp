#include "HotspotsEditor.h"
#include "HotspotsEditorDisabledState.h"
#include "HotspotsEditorSelectingState.h"
#include "ViewStateManager.h"

HotspotsEditorDisabledState::HotspotsEditorDisabledState(
        ViewStateManager *mgr, HotspotsEditor *editor, QObject *parent) :
    HotspotsEditorViewState(mgr, editor, parent)
{
}

void HotspotsEditorDisabledState::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_E) {
        m_mgr->moveTo(new HotspotsEditorSelectingState(m_mgr, m_editor));
        event->accept();
    }
}

void HotspotsEditorDisabledState::onEntry()
{
    m_editor->destroyHotspotsAreas();

    if (m_editor->m_navigator) {
        Navigator *nav = *(m_editor->m_navigator);
        if (nav) {
            disconnect(nav->currentImage(), SIGNAL(hotpotsLoaded()),
                       m_editor, SLOT(createHotspotsAreas()));
        }
    }
}
