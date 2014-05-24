#include <QGraphicsRectItem>

#include "HotspotsEditor.h"
#include "HotspotsEditorConfirmingState.h"
#include "HotspotsEditorDeletingState.h"
#include "HotspotsEditorDisabledState.h"
#include "HotspotsEditorSelectingState.h"
#include "ViewStateManager.h"

HotspotsEditorSelectingState::HotspotsEditorSelectingState(
        ViewStateManager *mgr, HotspotsEditor *editor, QObject *parent) :
    HotspotsEditorViewState(mgr, editor, parent)
{
}

void HotspotsEditorSelectingState::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        m_mgr->moveTo(new HotspotsEditorDisabledState(m_mgr, m_editor));
        event->accept();
    } else if (event->key() == Qt::Key_D) {
        m_mgr->moveTo(new HotspotsEditorDeletingState(m_mgr, m_editor));
        event->accept();
    }
}

void HotspotsEditorSelectingState::mouseMoveEvent(QMouseEvent *event)
{
    m_editor->setHotspotsSelectingAreaPos(event->pos());
    event->accept();
}

void HotspotsEditorSelectingState::mousePressEvent(QMouseEvent *event)
{
    m_mgr->moveTo(new HotspotsEditorConfirmingState(m_mgr, m_editor));
    event->accept();
}

void HotspotsEditorSelectingState::onEntry()
{
    Navigator *nav = *(m_editor->m_navigator);
    connect(nav->currentImage(), SIGNAL(hotpotsLoaded()),
            m_editor, SLOT(createHotspotsAreas()),
            static_cast<Qt::ConnectionType>(Qt::AutoConnection | Qt::UniqueConnection));

    m_editor->m_hotspotsSelectingArea->setBrush(
        HotspotsEditor::HotspotColorSelecting);
    m_editor->m_hotspotsSelectingArea->show();
}

void HotspotsEditorSelectingState::onExit()
{
    m_editor->m_hotspotsSelectingArea->hide();
}

