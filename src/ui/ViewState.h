#ifndef VIEWSTATE_H
#define VIEWSTATE_H

#include <QKeyEvent>
#include <QMouseEvent>
#include <QObject>

class ViewStateManager;

class ViewState : public QObject
{
    Q_OBJECT

    friend class ViewStateManager;

public:
    explicit ViewState(ViewStateManager *mgr, QObject *parent = 0);

    virtual void keyPressEvent(QKeyEvent *) {}
    virtual void mouseMoveEvent(QMouseEvent *) {}
    virtual void mousePressEvent(QMouseEvent *) {}

protected:
    virtual void onEntry() {}
    virtual void onExit() {}

    ViewStateManager *m_mgr;

};

#endif // VIEWSTATE_H
