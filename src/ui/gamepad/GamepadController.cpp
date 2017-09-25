#include "GamepadController.h"

#include "ui/gamepad/GamepadAxisScroller.h"
#include "ui/models/MainGraphicsViewModel.h"
#include "MainWindow.h"
#include "ui_MainWindow.h"

GamepadController::GamepadController(MainWindow *mainWindow) :
    m_mainWindow(mainWindow) ,
    m_gamepadLastL2(QTime::currentTime()) ,
    m_gamepadLastR2(QTime::currentTime()) ,
    m_axisLeftScroller(new GamepadAxisScroller())
{
    connect(&m_gamepad, &QGamepad::buttonL2Changed, this, &GamepadController::buttonL2Changed);
    connect(&m_gamepad, &QGamepad::buttonR2Changed, this, &GamepadController::buttonR2Changed);
    connect(&m_gamepad, &QGamepad::buttonYChanged, this, &GamepadController::buttonYChanged);
    connect(&m_gamepad, &QGamepad::buttonAChanged, this, &GamepadController::buttonAChanged);
    connect(&m_gamepad, &QGamepad::buttonBChanged, this, &GamepadController::buttonBChanged);
    connect(&m_gamepad, &QGamepad::buttonLeftChanged, this, &GamepadController::buttonLeftChanged);
    connect(&m_gamepad, &QGamepad::buttonRightChanged, this, &GamepadController::buttonRightChanged);
    connect(&m_gamepad, &QGamepad::buttonUpChanged, this, &GamepadController::buttonUpChanged);
    connect(&m_gamepad, &QGamepad::buttonDownChanged, this, &GamepadController::buttonDownChanged);
    connect(&m_gamepad, &QGamepad::buttonSelectChanged, this, &GamepadController::buttonSelectChanged);

    connect(&m_gamepad, &QGamepad::axisLeftXChanged, m_axisLeftScroller.data(), &GamepadAxisScroller::setAxisXValue);
    connect(&m_gamepad, &QGamepad::axisLeftYChanged, m_axisLeftScroller.data(), &GamepadAxisScroller::setAxisYValue);
    connect(m_axisLeftScroller.data(), &GamepadAxisScroller::scroll, this, &GamepadController::axisLeftScroll);
}

void GamepadController::buttonL2Changed(bool pressed)
{
    if (pressed) {
        QTime now = QTime::currentTime();
        if (m_gamepadLastL2.msecsTo(now) > GAMEPAD_DEBOUNCE_INTERVAL) {
            if (m_mainWindow->m_navigator->isAutoNavigating()) {
                m_mainWindow->m_navigator->stopAutoNavigation();
            } else {
                m_mainWindow->m_navigator->goPrev();
            }
            m_gamepadLastL2 = now;
        }
    }
}

void GamepadController::buttonR2Changed(bool pressed)
{
    if (pressed) {
        QTime now = QTime::currentTime();
        if (m_gamepadLastR2.msecsTo(now) > GAMEPAD_DEBOUNCE_INTERVAL) {
            if (m_mainWindow->m_navigator->isAutoNavigating()) {
                m_mainWindow->m_navigator->stopAutoNavigation();
            } else {
                m_mainWindow->m_navigator->goNext();
            }
            m_gamepadLastR2 = now;
        }
    }
}

void GamepadController::buttonLeftChanged(bool pressed)
{
    if (pressed) {
        m_mainWindow->moveCursor(Qt::Key_Left);
    }
}

void GamepadController::buttonRightChanged(bool pressed)
{
    if (pressed) {
        m_mainWindow->moveCursor(Qt::Key_Right);
    }
}

void GamepadController::buttonUpChanged(bool pressed)
{
    if (pressed) {
        m_mainWindow->moveCursor(Qt::Key_Up);
    }
}

void GamepadController::buttonDownChanged(bool pressed)
{
    if (pressed) {
        m_mainWindow->moveCursor(Qt::Key_Down);
    }
}

void GamepadController::buttonYChanged(bool pressed)
{
    if (pressed) {
        m_mainWindow->m_mainGraphicsViewModel->toggleFitInView();
        m_mainWindow->m_mainGraphicsViewModel->fitInViewIfNecessary();

        if (!m_mainWindow->m_secondaryMainGraphicsViewModel.isNull()) {
            m_mainWindow->m_secondaryMainGraphicsViewModel->toggleFitInView();
            m_mainWindow->m_secondaryMainGraphicsViewModel->fitInViewIfNecessary();
        }
    }
}

void GamepadController::buttonAChanged(bool pressed)
{
    if (pressed) {
        auto *ui = m_mainWindow->ui;
        if (ui->stackedViews->currentWidget() == ui->pageFav) {
            m_mainWindow->loadSelectedPlayList();
        } else if (ui->stackedViews->currentWidget() == ui->pageFileSystem) {
            m_mainWindow->loadOrEnterSelectedPath();
        } else if (ui->stackedViews->currentWidget() == ui->pageGallery) {
            m_mainWindow->m_actToolBarImage->trigger();
        }
    }
}

void GamepadController::buttonBChanged(bool pressed)
{
    if (pressed) {
        auto *ui = m_mainWindow->ui;
        if (ui->stackedViews->currentWidget() == ui->pageImageView) {
            m_mainWindow->m_actToolBarGallery->trigger();
        }
    }
}

void GamepadController::buttonSelectChanged(bool pressed)
{
    if (pressed) {
        m_mainWindow->switchViews();
    }
}

void GamepadController::axisLeftScroll(int dx, int dy)
{
    m_mainWindow->m_mainGraphicsViewModel->scroll(dx, dy);
}