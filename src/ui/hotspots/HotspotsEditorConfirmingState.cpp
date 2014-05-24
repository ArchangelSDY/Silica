#include <QGraphicsRectItem>

#include "HotspotsEditor.h"
#include "HotspotsEditorConfirmingState.h"
#include "HotspotsEditorSelectingState.h"

HotspotsEditorConfirmingState::HotspotsEditorConfirmingState(
        ViewStateManager *mgr, HotspotsEditor *editor, QObject *parent) :
    HotspotsEditorViewState(mgr, editor, parent) ,
    m_mask(0)
{
}

HotspotsEditorConfirmingState::~HotspotsEditorConfirmingState()
{
    if (m_mask) {
        m_editor->m_scene->removeItem(m_mask);
        delete m_mask;
    }
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
    if (m_mask) {
        m_editor->m_scene->removeItem(m_mask);
        delete m_mask;
    }

    QGraphicsRectItem *selectingArea = m_editor->m_selectingArea;
    QRectF selected(selectingArea->pos(), selectingArea->rect().size());

    QPolygonF shape(m_editor->m_scene->sceneRect());
    shape = shape.subtracted(QPolygonF(selected));

    m_mask = new QGraphicsPolygonItem(shape);
    m_mask->setBrush(HotspotsEditor::HotspotColorMask);
    m_editor->m_scene->addItem(m_mask);

    // Hide existed areas
    m_editor->m_hotspotsAreas->hide();
}

void HotspotsEditorConfirmingState::onExit()
{
    // m_editor->m_selectingArea->hide();
    if (m_mask) {
        m_editor->m_scene->removeItem(m_mask);
        delete m_mask;
        m_mask = 0;
    }

    // Show existed areas again
    m_editor->m_hotspotsAreas->show();
}
