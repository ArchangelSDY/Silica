#include <QGraphicsRectItem>
#include <QGraphicsView>

#include "HotspotsEditor.h"
#include "HotspotsEditorConfirmingState.h"
#include "HotspotsEditorDeletingState.h"
#include "HotspotsEditorDisabledState.h"
#include "HotspotsEditorSelectingState.h"
#include "Navigator.h"
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
    } else if (event->key() == Qt::Key_W) {
        scaleUpSelectingArea();
        event->accept();
    } else if (event->key() == Qt::Key_N) {
        scaleDownSelectingArea();
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
    nav->currentImage()->loadHotspots(true);

    setSelectingAreaSize();

    m_editor->m_selectingArea->setBrush(
        HotspotsEditor::HotspotColorSelecting);
    m_editor->m_selectingArea->show();
}

void HotspotsEditorSelectingState::onExit()
{
    m_editor->m_selectingArea->hide();
    m_editor->m_selectingAreaExpanding = 0; // Reset before editing next image
}

void HotspotsEditorSelectingState::setSelectingAreaSize()
{
    QGraphicsScene *scene = m_editor->m_scene;
    QGraphicsView *view = scene->views()[0];

    QSize size = view->size();
    size += QSize(m_editor->m_selectingAreaExpanding,
                  m_editor->m_selectingAreaExpanding);

    // Hotspot must be bigger than view, or image will be over scaled.
    size = size.expandedTo(view->size());

    // Hotspot must be smaller than scene, or image cannot fulfill the view.
    QSize sceneSize = scene->sceneRect().toAlignedRect().size();
    size = size.boundedTo(sceneSize);

    m_editor->m_selectingArea->setRect(QRect(QPoint(), size));
}

void HotspotsEditorSelectingState::scaleUpSelectingArea()
{
    m_editor->m_selectingAreaExpanding += selectingAreaScaleDelta();
    setSelectingAreaSize();
}

void HotspotsEditorSelectingState::scaleDownSelectingArea()
{
    m_editor->m_selectingAreaExpanding -= selectingAreaScaleDelta();
    setSelectingAreaSize();
}

int HotspotsEditorSelectingState::selectingAreaScaleDelta()
{
    Image *image = (*(m_editor->m_navigator))->currentImage();
    return image->data().width() / 15;
}

