#include <QGraphicsRectItem>
#include <QGraphicsView>
#include <QMessageBox>

#include "HotspotsEditor.h"
#include "HotspotsEditorDisabledState.h"
#include "ImageHotspot.h"

const QColor HotspotsEditor::HotspotColorDefault = QColor("#AA0000AA");
const QColor HotspotsEditor::HotspotColorSelecting = QColor("#AA00AAAA");
const QColor HotspotsEditor::HotspotColorFocused = QColor("#AAAA0000");
const QColor HotspotsEditor::HotspotColorMask = QColor("#AA000000");

HotspotsEditor::HotspotsEditor(QGraphicsScene *scene, Navigator **navigator) :
    m_scene(scene) ,
    m_navigator(navigator) ,
    m_statesMgr(new HotspotsEditorViewStateManager(this)) ,
    m_selectingArea(new QGraphicsRectItem()) ,
    m_hotspotsAreas(0) ,
    m_selectingAreaExpanding(0)
{
    m_selectingArea->hide();
    m_selectingArea->setZValue(10);     // Make it on the top
    m_scene->addItem(m_selectingArea);

    m_statesMgr->moveTo(new HotspotsEditorDisabledState(m_statesMgr, this));
}

void HotspotsEditor::navigationChange(int index)
{
    m_statesMgr->navigationChange(index);
}

void HotspotsEditor::keyPressEvent(QKeyEvent *event)
{
    m_statesMgr->keyPressEvent(event);
}

void HotspotsEditor::mouseMoveEvent(QMouseEvent *event)
{
    m_statesMgr->mouseMoveEvent(event);
}

void HotspotsEditor::mousePressEvent(QMouseEvent *event)
{
    m_statesMgr->mousePressEvent(event);
}

void HotspotsEditor::createHotspotsAreas()
{
    if (!m_navigator || !(*m_navigator)) {
        return;
    }

    destroyHotspotsAreas();

    QList<ImageHotspot *> hotspots = (*m_navigator)->currentImage()->hotspots();
    QList<QGraphicsItem *> hotspotsAreas;
    foreach (ImageHotspot *hotspot, hotspots) {
        QGraphicsRectItem *item = new QGraphicsRectItem(hotspot->rect());
        item->setBrush(QColor(HotspotColorDefault));
        hotspotsAreas << item;
    }
    m_hotspotsAreas = m_scene->createItemGroup(hotspotsAreas);
}

void HotspotsEditor::setHotspotsSelectingAreaPos(const QPoint &rawPos)
{
    Q_ASSERT(m_scene->views().count() > 0);

    QGraphicsView *view = m_scene->views()[0];
    QPointF pos = view->mapToScene(rawPos);

    QRectF rect = m_selectingArea->rect();
    QRectF sceneRect = m_scene->sceneRect();

    rect.moveTo(pos - m_selectingArea->rect().center());

    if (rect.left() < 0) {
        rect.setLeft(0);
    }
    if (rect.right() > sceneRect.right()) {
        rect.moveRight(sceneRect.width());
    }
    if (rect.top() < 0) {
        rect.setTop(0);
    }
    if (rect.bottom() > sceneRect.bottom()) {
        rect.moveBottom(sceneRect.bottom());
    }

    m_selectingArea->setPos(rect.topLeft());
}

void HotspotsEditor::destroyHotspotsAreas()
{
    if (m_hotspotsAreas) {
        foreach (QGraphicsItem *item, m_hotspotsAreas->childItems()) {
            m_scene->removeItem(item);
        }
        m_scene->destroyItemGroup(m_hotspotsAreas);
        m_hotspotsAreas = 0;
    }
}

void HotspotsEditor::confirmHotspotDeleting(const QPoint &pos)
{
    Q_ASSERT(m_scene->views().count() > 0);

    QGraphicsView *view = m_scene->views()[0];
    QPointF realPos = view->mapToScene(pos);
    QGraphicsItem *rawItem = m_scene->itemAt(realPos.toPoint(),
                                             view->transform());

    if (rawItem && rawItem->type() == QGraphicsRectItem::Type) {
        QGraphicsRectItem *item = static_cast<QGraphicsRectItem *>(rawItem);
        item->setBrush(HotspotColorFocused);

        if (QMessageBox::question(view, tr("Removing Hotspot"),
                tr("Remove this?")) == QMessageBox::Yes) {
            QList<ImageHotspot *> hotspots =
                (*m_navigator)->currentImage()->hotspots();
            for (int i = 0; i < hotspots.count(); ++i) {
                if (hotspots[i]->rect() == item->rect().toAlignedRect()) {
                    hotspots[i]->remove();
                    break;
                }
            }

            // Refresh areas
            createHotspotsAreas();
        } else {
            item->setBrush(HotspotColorDefault);
        }
    }
}

void HotspotsEditor::saveHotspot()
{
    QRectF rect = m_selectingArea->rect();
    rect.moveTo(m_selectingArea->pos());

    ImageHotspot hotspot((*m_navigator)->currentImage(), rect.toAlignedRect());
    hotspot.save();
}
