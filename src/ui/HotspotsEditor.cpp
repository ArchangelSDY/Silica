#include <QGraphicsRectItem>
#include <QGraphicsView>
#include <QMessageBox>

#include "HotspotsEditor.h"
#include "ImageHotspot.h"

const QColor HotspotsEditor::HotspotColorDefault = QColor("#AA0000AA");
const QColor HotspotsEditor::HotspotColorSelecting = QColor("#AA00AAAA");
const QColor HotspotsEditor::HotspotColorFocused = QColor("#AAAA0000");

HotspotsEditor::HotspotsEditor(QGraphicsScene *scene, Navigator **navigator) :
    m_scene(scene) ,
    m_navigator(navigator) ,
    m_hotspotsEditingState(HotspotsNotEditing) ,
    m_hotspotsSelectingArea(new QGraphicsRectItem()) ,
    m_hotspotsAreas(0)
{
    QSizeF defaultHotspotSize = qApp->primaryScreen()->size() / 3;
    m_hotspotsSelectingArea->setRect(QRectF(QPointF(), defaultHotspotSize));
    m_hotspotsSelectingArea->setBrush(HotspotColorSelecting);
    m_hotspotsSelectingArea->hide();
    m_hotspotsSelectingArea->setZValue(10);     // Make it on the top
    m_scene->addItem(m_hotspotsSelectingArea);
}

void HotspotsEditor::keyPressEvent(QKeyEvent *event)
{
    if (!isEditing()) {
        if (event->key() == Qt::Key_E) {                // Press 'E' to enter editing
            enterHotspotsEditing();
            event->accept();
        }
    } else {
        if (event->key() == Qt::Key_Escape) {           // Press 'Q' to quit
            if (m_hotspotsEditingState == HotspotsSelecting) {
                leaveHotspotsEditing();
                event->accept();
            } else if (m_hotspotsEditingState == HotspotsConfirming) {
                leaveHotspotsConfirming();
                event->accept();
            } else if (m_hotspotsEditingState == HotspotsDeleting) {
                leaveHotspotsDeleting();
                event->accept();
            }
        } else if (event->key() == Qt::Key_Return) {    // Press 'Return' to commit
            if (m_hotspotsEditingState == HotspotsConfirming) {
                // Save area
                saveHotspot();
                leaveHotspotsConfirming();
                event->accept();
            }
        } else if (event->key() == Qt::Key_D) {         // Press 'D' to enter deleting mode
            if (m_hotspotsEditingState == HotspotsSelecting) {
                enterHotspotsDeleting();
                event->accept();
            }
        }
    }
}

void HotspotsEditor::mouseMoveEvent(QMouseEvent *event)
{
    if (m_hotspotsEditingState == HotspotsSelecting) {
        Q_ASSERT(m_scene->views().count() > 0);

        QGraphicsView *view = m_scene->views()[0];
        QPointF pos = view->mapToScene(event->pos());
        setHotspotsSelectingAreaPos(pos);
        event->accept();
    }
}

void HotspotsEditor::mousePressEvent(QMouseEvent *event)
{
    if (m_hotspotsEditingState == HotspotsSelecting) {
        enterHotspotsConfirming();
        event->accept();
    } else if (m_hotspotsEditingState == HotspotsDeleting) {
        confirmHotspotDeleting(event->pos());
        event->accept();
    }
}

void HotspotsEditor::createHotspotsAreas()
{
    if (!m_navigator || !*m_navigator) {
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

void HotspotsEditor::setHotspotsSelectingAreaPos(const QPointF &pos)
{
    QRectF rect = m_hotspotsSelectingArea->rect();
    QRectF sceneRect = m_scene->sceneRect();

    rect.moveTo(pos - m_hotspotsSelectingArea->rect().center());

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

    m_hotspotsSelectingArea->setPos(rect.topLeft());
}


void HotspotsEditor::enterHotspotsEditing()
{
    m_hotspotsEditingState = HotspotsSelecting;

    connect((*m_navigator)->currentImage(), SIGNAL(hotpotsLoaded()),
            this, SLOT(createHotspotsAreas()),
            static_cast<Qt::ConnectionType>(Qt::AutoConnection | Qt::UniqueConnection));
    (*m_navigator)->currentImage()->loadHotspots();

    m_hotspotsSelectingArea->show();
}

void HotspotsEditor::leaveHotspotsEditing()
{
    m_hotspotsEditingState = HotspotsNotEditing;
    m_hotspotsSelectingArea->hide();

    destroyHotspotsAreas();

    disconnect((*m_navigator)->currentImage(), SIGNAL(hotpotsLoaded()),
               this, SLOT(createHotspotsAreas()));
}

void HotspotsEditor::enterHotspotsConfirming()
{
    m_hotspotsEditingState = HotspotsConfirming;
    m_hotspotsSelectingArea->setBrush(HotspotColorFocused);
}

void HotspotsEditor::leaveHotspotsConfirming()
{
    m_hotspotsEditingState = HotspotsSelecting;
    m_hotspotsSelectingArea->setBrush(HotspotColorSelecting);
}

void HotspotsEditor::enterHotspotsDeleting()
{
    m_hotspotsSelectingArea->hide();
    m_hotspotsEditingState = HotspotsDeleting;
}

void HotspotsEditor::leaveHotspotsDeleting()
{
    m_hotspotsSelectingArea->show();
    m_hotspotsEditingState = HotspotsSelecting;
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
    QRectF rect = m_hotspotsSelectingArea->rect();
    rect.moveTo(m_hotspotsSelectingArea->pos());

    ImageHotspot hotspot((*m_navigator)->currentImage(), rect.toAlignedRect());
    hotspot.save();
}
