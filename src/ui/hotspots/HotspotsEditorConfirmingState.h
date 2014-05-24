#ifndef HOTSPOTSEDITORCONFIRMINGSTATE_H
#define HOTSPOTSEDITORCONFIRMINGSTATE_H

#include <QGraphicsPolygonItem>

#include "HotspotsEditorViewState.h"

class HotspotsEditorConfirmingState : public HotspotsEditorViewState
{
    Q_OBJECT
public:
    explicit HotspotsEditorConfirmingState(ViewStateManager *mgr,
                                           HotspotsEditor *editor,
                                           QObject *parent = 0);
    ~HotspotsEditorConfirmingState();

    void keyPressEvent(QKeyEvent *event);

protected:

    void onEntry();
    void onExit();

private:
    QGraphicsPolygonItem *m_mask;
};

#endif // HOTSPOTSEDITORCONFIRMINGSTATE_H
