#ifndef HOTSPOTSEDITOR_H
#define HOTSPOTSEDITOR_H

#include <QGraphicsScene>

#include "Navigator.h"

class HotspotsEditor : public QObject
{
    Q_OBJECT
public:
    HotspotsEditor(QGraphicsScene *scene, Navigator **navigator);

    bool isEditing() const { return m_hotspotsEditingState != HotspotsNotEditing; }

    void keyPressEvent(QKeyEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);

    void enterHotspotsEditing();
    void leaveHotspotsEditing();

private slots:
    void createHotspotsAreas();

private:
    enum HotspotsEditingState {
        HotspotsNotEditing,
        HotspotsSelecting,
        HotspotsConfirming,
        HotspotsDeleting,
    };

    void setHotspotsSelectingAreaPos(const QPointF &pos);
    void enterHotspotsConfirming();
    void leaveHotspotsConfirming();
    void enterHotspotsDeleting();
    void leaveHotspotsDeleting();
    void destroyHotspotsAreas();
    void confirmHotspotDeleting(const QPointF &pos);
    void saveHotspot();

    QGraphicsScene *m_scene;
    Navigator **m_navigator;
    HotspotsEditingState m_hotspotsEditingState;
    QGraphicsRectItem *m_hotspotsSelectingArea;
    QGraphicsItemGroup *m_hotspotsAreas;
};

#endif // HOTSPOTSEDITOR_H
