#include "ViewState.h"

ViewState::ViewState(ViewStateManager *mgr, QObject *parent) :
    QObject(parent) ,
    m_mgr(mgr)
{
}
