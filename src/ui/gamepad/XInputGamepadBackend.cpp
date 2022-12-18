/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt Gamepad module
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include <QtCore/QThread>
#include <qmath.h>
#include <windows.h>

#include "XInputGamepadBackend.h"

#define POLL_SLEEP_MS 5
#define POLL_SLOT_CHECK_MS 4000

#define XUSER_MAX_COUNT 4

#define XINPUT_GAMEPAD_DPAD_UP        0x0001
#define XINPUT_GAMEPAD_DPAD_DOWN      0x0002
#define XINPUT_GAMEPAD_DPAD_LEFT      0x0004
#define XINPUT_GAMEPAD_DPAD_RIGHT     0x0008
#define XINPUT_GAMEPAD_START          0x0010
#define XINPUT_GAMEPAD_BACK           0x0020
#define XINPUT_GAMEPAD_LEFT_THUMB     0x0040
#define XINPUT_GAMEPAD_RIGHT_THUMB    0x0080
#define XINPUT_GAMEPAD_LEFT_SHOULDER  0x0100
#define XINPUT_GAMEPAD_RIGHT_SHOULDER 0x0200
#define XINPUT_GAMEPAD_A              0x1000
#define XINPUT_GAMEPAD_B              0x2000
#define XINPUT_GAMEPAD_X              0x4000
#define XINPUT_GAMEPAD_Y              0x8000

#define XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE  7849
#define XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE 8689
#define XINPUT_GAMEPAD_TRIGGER_THRESHOLD    30

struct XINPUT_GAMEPAD
{
    unsigned short wButtons;
    unsigned char bLeftTrigger;
    unsigned char bRightTrigger;
    short sThumbLX;
    short sThumbLY;
    short sThumbRX;
    short sThumbRY;
};

struct XINPUT_STATE
{
    unsigned long dwPacketNumber;
    XINPUT_GAMEPAD Gamepad;
};

typedef DWORD (WINAPI *XInputGetState_t)(DWORD dwUserIndex, XINPUT_STATE *pState);
static XInputGetState_t XInputGetState;

class XInputThread : public QThread
{
public:
    XInputThread(XInputGamepadBackend *backend);
    void run() override;
    void signalQuit() { m_quit.fetchAndStoreAcquire(1); }

private:
    void dispatch(int idx, XINPUT_GAMEPAD *state);

    XInputGamepadBackend *m_backend;
    QAtomicInt m_quit;
    struct Controller {
        bool connected;
        int skippedPolls;
        unsigned long lastPacketNumber;
        // State cache. Only want to emit signals for values that really change.
        unsigned short buttons;
        unsigned char triggers[2];
        double axis[2][2];
    } m_controllers[XUSER_MAX_COUNT];
};

XInputThread::XInputThread(XInputGamepadBackend *backend)
    : m_backend(backend),
      m_quit(false)
{
    memset(m_controllers, 0, sizeof(m_controllers));
}

void XInputThread::dispatch(int idx, XINPUT_GAMEPAD *state)
{
    static const struct ButtonMap {
        unsigned short xbutton;
        GamepadBackend::Button qbutton;
    } buttonMap[] = {
        { XINPUT_GAMEPAD_DPAD_UP, GamepadBackend::Button::Up },
        { XINPUT_GAMEPAD_DPAD_DOWN, GamepadBackend::Button::Down },
        { XINPUT_GAMEPAD_DPAD_LEFT, GamepadBackend::Button::Left },
        { XINPUT_GAMEPAD_DPAD_RIGHT, GamepadBackend::Button::Right },
        { XINPUT_GAMEPAD_START, GamepadBackend::Button::Start },
        { XINPUT_GAMEPAD_BACK, GamepadBackend::Button::Select },
        { XINPUT_GAMEPAD_LEFT_SHOULDER, GamepadBackend::Button::L1 },
        { XINPUT_GAMEPAD_RIGHT_SHOULDER, GamepadBackend::Button::R1 },
        { XINPUT_GAMEPAD_LEFT_THUMB, GamepadBackend::Button::L3 },
        { XINPUT_GAMEPAD_RIGHT_THUMB, GamepadBackend::Button::R3 },
        { XINPUT_GAMEPAD_A, GamepadBackend::Button::A },
        { XINPUT_GAMEPAD_B, GamepadBackend::Button::B },
        { XINPUT_GAMEPAD_X, GamepadBackend::Button::X },
        { XINPUT_GAMEPAD_Y, GamepadBackend::Button::Y }
    };
    for (uint i = 0; i < sizeof(buttonMap) / sizeof(ButtonMap); ++i) {
        const unsigned short xb = buttonMap[i].xbutton;
        unsigned short isDown = state->wButtons & xb;
        if (isDown != (m_controllers[idx].buttons & xb)) {
            if (isDown) {
                m_controllers[idx].buttons |= xb;
                emit m_backend->buttonPressed(idx, buttonMap[i].qbutton, 1);
            } else {
                m_controllers[idx].buttons &= ~xb;
                emit m_backend->buttonReleased(idx, buttonMap[i].qbutton);
            }
        }
    }

    if (m_controllers[idx].triggers[0] != state->bLeftTrigger) {
        m_controllers[idx].triggers[0] = state->bLeftTrigger;
        const double value = state->bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD
                ? (state->bLeftTrigger - XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
                  / (255.0 - XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
                : 0.0;
        if (!qFuzzyIsNull(value))
            emit m_backend->buttonPressed(idx, GamepadBackend::Button::L2, value);
        else
            emit m_backend->buttonReleased(idx, GamepadBackend::Button::L2);
    }
    if (m_controllers[idx].triggers[1] != state->bRightTrigger) {
        m_controllers[idx].triggers[1] = state->bRightTrigger;
        const double value = state->bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD
                ? (state->bRightTrigger - XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
                  / (255.0 - XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
                : 0.0;
        if (!qFuzzyIsNull(value))
            emit m_backend->buttonPressed(idx, GamepadBackend::Button::R2, value);
        else
            emit m_backend->buttonReleased(idx, GamepadBackend::Button::R2);
    }

    double x, y;
    if (qSqrt(state->sThumbLX * state->sThumbLX + state->sThumbLY * state->sThumbLY) > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) {
        x = 2 * (state->sThumbLX + 32768.0) / 65535.0 - 1.0;
        y = 2 * (-state->sThumbLY + 32768.0) / 65535.0 - 1.0;
    } else {
        x = y = 0;
    }
    if (m_controllers[idx].axis[0][0] != x) {
        m_controllers[idx].axis[0][0] = x;
        emit m_backend->axisMoved(idx, GamepadBackend::Axis::LeftX, x);
    }
    if (m_controllers[idx].axis[0][1] != y) {
        m_controllers[idx].axis[0][1] = y;
        emit m_backend->axisMoved(idx, GamepadBackend::Axis::LeftY, y);
    }
    if (qSqrt(state->sThumbRX * state->sThumbRX + state->sThumbRY * state->sThumbRY) > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) {
        x = 2 * (state->sThumbRX + 32768.0) / 65535.0 - 1.0;
        y = 2 * (-state->sThumbRY + 32768.0) / 65535.0 - 1.0;
    } else {
        x = y = 0;
    }
    if (m_controllers[idx].axis[1][0] != x) {
        m_controllers[idx].axis[1][0] = x;
        emit m_backend->axisMoved(idx, GamepadBackend::Axis::RightX, x);
    }
    if (m_controllers[idx].axis[1][1] != y) {
        m_controllers[idx].axis[1][1] = y;
        emit m_backend->axisMoved(idx, GamepadBackend::Axis::RightY, y);
    }
}

void XInputThread::run()
{
    bool firstPoll = true;
    while (!m_quit.testAndSetAcquire(1, 0)) {
        for (int i = 0; i < XUSER_MAX_COUNT; ++i) {
            Controller *controller = m_controllers + i;

            if (!firstPoll && !controller->connected && controller->skippedPolls < POLL_SLOT_CHECK_MS / POLL_SLEEP_MS) {
                controller->skippedPolls++;
                continue;
            }

            firstPoll = false;
            controller->skippedPolls = 0;
            XINPUT_STATE state;
            memset(&state, 0, sizeof(state));

            if (XInputGetState(i, &state) == ERROR_SUCCESS) {
                if (controller->connected) {
                    if (controller->lastPacketNumber != state.dwPacketNumber) {
                        controller->lastPacketNumber = state.dwPacketNumber;
                        dispatch(i, &state.Gamepad);
                    }
                } else {
                    controller->connected = true;
                    controller->lastPacketNumber = state.dwPacketNumber;
                    emit m_backend->gamepadAdded(i);
                    dispatch(i, &state.Gamepad);
                }
            } else {
                if (controller->connected) {
                    controller->connected = false;
                    emit m_backend->gamepadRemoved(i);
                }
            }
        }

        Sleep(POLL_SLEEP_MS);
    }
}

XInputGamepadBackend::XInputGamepadBackend(QObject *parent)
    : GamepadBackend(parent)
    , m_thread(0)
{
}

bool XInputGamepadBackend::start()
{
    m_lib.setFileName(QStringLiteral("xinput1_4.dll"));
    if (!m_lib.load()) {
        m_lib.setFileName(QStringLiteral("xinput1_3.dll"));
        m_lib.load();
    }

    if (m_lib.isLoaded()) {
        XInputGetState = (XInputGetState_t) m_lib.resolve("XInputGetState");
        if (XInputGetState) {
            m_thread = new XInputThread(this);
            m_thread->start();
        } else {
            qWarning("Failed to resolve XInputGetState");
        }
    } else {
        qWarning("Failed to load XInput library %s", qPrintable(m_lib.fileName()));
    }

    return m_lib.isLoaded();
}

void XInputGamepadBackend::stop()
{
    m_thread->signalQuit();
    m_thread->wait();
    XInputGetState = 0;
}
