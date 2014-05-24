#include <QGraphicsRectItem>

#include "HotspotsEditor.h"
#include "HotspotsEditorConfirmingState.h"
#include "HotspotsEditorSelectingState.h"

HotspotsEditorConfirmingState::HotspotsEditorConfirmingState(
        ViewStateManager *mgr, HotspotsEditor *editor, QObject *parent) :
    HotspotsEditorViewState(mgr, editor, parent)
{
}

void HotspotsEditorConfirmingState::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return) {
        m_editor->saveHotspot();
        m_mgr->moveTo(new HotspotsEditorSelectingState(m_mgr, m_editor));
        event->accept();
    } else if (event->key() == Qt::Key_Escape) {
        m_mgr->moveTo(new HotspotsEditorSelectingState(m_mgr, m_editor));
        event->accept();
    }
}

void HotspotsEditorConfirmingState::onEntry()
{
    m_editor->m_selectingArea->setBrush(
        HotspotsEditor::HotspotColorFocused);
    m_editor->m_selectingArea->show();
}

void HotspotsEditorConfirmingState::onExit()
{
    m_editor->m_selectingArea->hide();
}
