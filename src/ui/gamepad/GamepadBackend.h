#pragma once

#include <QObject>

class GamepadBackend : public QObject
{
    Q_OBJECT
public:
    virtual ~GamepadBackend();

    virtual bool start() = 0;
    virtual void stop() = 0;

    enum class Button {
        Up,
        Down,
        Left,
        Right,
        Start,
        Select,
        L1,
        R1,
        L2,
        R2,
        L3,
        R3,
        A,
        B,
        X,
        Y,
    };

    enum class Axis {
        LeftX,
        LeftY,
        RightX,
        RightY,
    };

signals:
    void buttonPressed(int index, Button button, double val);
    void buttonReleased(int index, Button button);
    void axisMoved(int index, Axis axis, double val);
    void gamepadAdded(int index);
    void gamepadRemoved(int index);
};