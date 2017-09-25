#pragma once

#include <QGamepad>
#include <QScopedPointer>
#include <QTime>

class MainWindow;
class GamepadAxisScroller;

class GamepadController : public QObject
{
    Q_OBJECT
public:
    GamepadController(MainWindow *mainWindow);

private slots:
    void buttonL2Changed(bool pressed);
    void buttonR2Changed(bool pressed);
    void buttonLeftChanged(bool pressed);
    void buttonRightChanged(bool pressed);
    void buttonUpChanged(bool pressed);
    void buttonDownChanged(bool pressed);
    void buttonYChanged(bool pressed);
    void buttonAChanged(bool pressed);
    void buttonBChanged(bool pressed);
    void buttonSelectChanged(bool pressed);
    void axisLeftScroll(int dx, int dy);

private:
    MainWindow *m_mainWindow;

    static const int GAMEPAD_DEBOUNCE_INTERVAL = 250;
    QGamepad m_gamepad;
    QTime m_gamepadLastL2;
    QTime m_gamepadLastR2;
    QScopedPointer<GamepadAxisScroller> m_axisLeftScroller;
};
