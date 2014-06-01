#include "HotspotsEditor.h"
#include "HotspotsEditorDeletingState.h"
#include "HotspotsEditorSelectingState.h"
#include "ViewStateManager.h"

HotspotsEditorDeletingState::HotspotsEditorDeletingState(
        ViewStateManager *mgr, HotspotsEditor *editor, QObject *parent) :
    HotspotsEditorViewState(mgr, editor, parent)
{
}

void HotspotsEditorDeletingState::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        m_mgr->moveTo(new HotspotsEditorSelectingState(m_mgr, m_editor));
        event->accept();
    }
}

void HotspotsEditorDeletingState::mousePressEvent(QMouseEvent *event)
{
    m_editor->confirmHotspotDeleting(event->pos());
    event->accept();
}

void HotspotsEditorDeletingState::onEntry()
{
    m_editor->m_indicator->setMessage("Hotspots Editor Deleting");
    m_editor->m_indicator->show();
}

void HotspotsEditorDeletingState::onExit()
{
    m_editor->m_indicator->hide();
}
