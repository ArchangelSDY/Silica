#include "ViewStateManager.h"

ViewStateManager::ViewStateManager(QObject *parent) :
    QObject(parent) ,
    m_curState(0)
{
}

void ViewStateManager::keyPressEvent(QKeyEvent *event)
{
    if (m_curState) {
        m_curState->keyPressEvent(event);
    }
}

void ViewStateManager::mouseMoveEvent(QMouseEvent *event)
{
    if (m_curState) {
        m_curState->mouseMoveEvent(event);
    }
}

void ViewStateManager::mousePressEvent(QMouseEvent *event)
{
    if (m_curState) {
        m_curState->mousePressEvent(event);
    }
}

void ViewStateManager::moveTo(ViewState *nextState)
{
    if (!nextState) {
        return;
    }

    if (m_curState) {
        m_curState->onExit();
        m_curState->deleteLater();
        m_curState = 0;
    }

    nextState->onEntry();
    m_curState = nextState;
}
