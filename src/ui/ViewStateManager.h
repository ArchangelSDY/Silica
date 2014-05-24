#ifndef VIEWSTATEMANAGER_H
#define VIEWSTATEMANAGER_H

#include <QKeyEvent>
#include <QMouseEvent>
#include <QObject>

#include "ViewState.h"

class ViewStateManager : public QObject
{
    Q_OBJECT
public:
    explicit ViewStateManager(QObject *parent = 0);

    void keyPressEvent(QKeyEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);

    void moveTo(ViewState *nextState);

private:
    ViewState *m_curState;
};

#endif // VIEWSTATEMANAGER_H
