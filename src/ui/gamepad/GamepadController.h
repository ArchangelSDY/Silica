#pragma once

#include <QScopedPointer>
#include <QTime>

#include "ui/gamepad/GamepadAxisScroller.h"
#include "ui/gamepad/GamepadBackend.h"

class MainWindow;
class GamepadAxisScroller;

class GamepadController : public QObject
{
    Q_OBJECT
public:
    GamepadController(MainWindow *mainWindow);

private:
    void buttonChanged(GamepadBackend::Button button, bool pressed);
    void buttonR1Changed(bool pressed);
    void buttonL2Changed(bool pressed);
    void buttonR2Changed(bool pressed);
    void buttonLeftChanged(bool pressed);
    void buttonRightChanged(bool pressed);
    void buttonUpChanged(bool pressed);
    void buttonDownChanged(bool pressed);
    void buttonXChanged(bool pressed);
    void buttonYChanged(bool pressed);
    void buttonAChanged(bool pressed);
    void buttonBChanged(bool pressed);
    void buttonSelectChanged(bool pressed);
    void axisMoved(GamepadBackend::Axis axis, double val);
    void axisLeftScroll(int dx, int dy);
    void axisRightScroll(int dx, int dy);

    MainWindow *m_mainWindow;

    static const int GAMEPAD_DEBOUNCE_INTERVAL = 250;
    QTime m_gamepadLastL2;
    QTime m_gamepadLastR2;
    QScopedPointer<GamepadAxisScroller> m_axisLeftScroller;
    QScopedPointer<GamepadAxisScroller> m_axisRightScroller;
    QScopedPointer<GamepadBackend> m_backend;
};
