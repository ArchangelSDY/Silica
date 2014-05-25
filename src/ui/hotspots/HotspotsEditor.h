#ifndef HOTSPOTSEDITOR_H
#define HOTSPOTSEDITOR_H

#include <QGraphicsScene>

#include "HotspotsEditorViewState.h"
#include "Navigator.h"
#include "HotspotsEditorViewStateManager.h"

class HotspotsEditor : public QObject
{
    Q_OBJECT

    friend class HotspotsEditorDisabledState;
    friend class HotspotsEditorSelectingState;
    friend class HotspotsEditorConfirmingState;
    friend class HotspotsEditorDeletingState;

public:
    HotspotsEditor(QGraphicsScene *scene, Navigator **navigator);

    void keyPressEvent(QKeyEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);

    void enterHotspotsEditing();
    void leaveHotspotsEditing();

public slots:
    void navigationChange(int);

private slots:
    void createHotspotsAreas();

private:

    static const QColor HotspotColorDefault;
    static const QColor HotspotColorSelecting;
    static const QColor HotspotColorFocused;
    static const QColor HotspotColorMask;

    void setHotspotsSelectingAreaPos(const QPoint &pos);
    void destroyHotspotsAreas();
    void confirmHotspotDeleting(const QPoint &pos);
    void saveHotspot();

    QGraphicsScene *m_scene;
    Navigator **m_navigator;

    HotspotsEditorViewStateManager *m_statesMgr;
    QGraphicsRectItem *m_selectingArea;
    QGraphicsItemGroup *m_hotspotsAreas;
    int m_selectingAreaExpanding;
};

#endif // HOTSPOTSEDITOR_H
