#include "GamepadController.h"

#include "ui/models/MainGraphicsViewModel.h"
#include "MainWindow.h"
#include "ui_MainWindow.h"

#ifdef Q_OS_WIN
#include "ui/gamepad/XInputGamepadBackend.h"
#endif

GamepadController::GamepadController(MainWindow *mainWindow) :
    m_mainWindow(mainWindow) ,
    m_gamepadLastL2(QTime::currentTime()) ,
    m_gamepadLastR2(QTime::currentTime()) ,
    m_axisLeftScroller(new GamepadAxisScroller()) ,
    m_axisRightScroller(new GamepadAxisScroller())
{
#ifdef Q_OS_WIN
    m_backend.reset(new XInputGamepadBackend());
#endif

    if (m_backend) {
        connect(m_backend.data(), &GamepadBackend::buttonPressed, [this](int index, GamepadBackend::Button button, double val) {
            this->buttonChanged(button, true);
        });
        connect(m_backend.data(), &GamepadBackend::buttonReleased, [this](int index, GamepadBackend::Button button) {
            this->buttonChanged(button, false);
        });
        connect(m_backend.data(), &GamepadBackend::axisMoved, [this](int index, GamepadBackend::Axis axis, double val) {
            this->axisMoved(axis, false);
        });
        connect(m_axisLeftScroller.data(), &GamepadAxisScroller::scroll, this, &GamepadController::axisLeftScroll);
        connect(m_axisRightScroller.data(), &GamepadAxisScroller::scroll, this, &GamepadController::axisRightScroll);
    }
}

void GamepadController::buttonChanged(GamepadBackend::Button button, bool pressed)
{
    switch (button) {
    case GamepadBackend::Button::R1:
        buttonR1Changed(pressed);
        break;

    case GamepadBackend::Button::L2:
        buttonL2Changed(pressed);
        break;

    case GamepadBackend::Button::R2:
        buttonR2Changed(pressed);
        break;

    case GamepadBackend::Button::X:
        buttonXChanged(pressed);
        break;

    case GamepadBackend::Button::Y:
        buttonXChanged(pressed);
        break;

    case GamepadBackend::Button::A:
        buttonXChanged(pressed);
        break;

    case GamepadBackend::Button::B:
        buttonXChanged(pressed);
        break;

    case GamepadBackend::Button::Left:
        buttonLeftChanged(pressed);
        break;

    case GamepadBackend::Button::Right:
        buttonRightChanged(pressed);
        break;

    case GamepadBackend::Button::Up:
        buttonUpChanged(pressed);
        break;

    case GamepadBackend::Button::Down:
        buttonDownChanged(pressed);
        break;

    case GamepadBackend::Button::Select:
        buttonSelectChanged(pressed);
        break;
    }
}

void GamepadController::axisMoved(GamepadBackend::Axis axis, double val)
{
    switch (axis) {
    case GamepadBackend::Axis::LeftX:
        m_axisLeftScroller->setAxisXValue(val);
        break;

    case GamepadBackend::Axis::LeftY:
        m_axisLeftScroller->setAxisYValue(val);
        break;

    case GamepadBackend::Axis::RightX:
        m_axisRightScroller->setAxisXValue(val);
        break;

    case GamepadBackend::Axis::RightY:
        m_axisRightScroller->setAxisYValue(val);
        break;
    }
}

void GamepadController::buttonR1Changed(bool pressed)
{
    if (pressed) {
        auto *ui = m_mainWindow->ui;
        if (ui->stackedViews->currentWidget() == ui->pageGallery) {
            ui->sideView->setVisible(!ui->sideView->isVisible());
        } else if (ui->stackedViews->currentWidget() == ui->pageImageView) {
            m_mainWindow->toggleSecondaryNavigator();
        }
    }
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

void GamepadController::buttonXChanged(bool pressed)
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

void GamepadController::buttonYChanged(bool pressed)
{
    if (pressed) {
        auto *ui = m_mainWindow->ui;
        QWidget *currentWidget = ui->stackedViews->currentWidget();
        if (currentWidget == ui->pageImageView) {
            m_mainWindow->m_navigator->goPrev();
        }
    }
}

void GamepadController::buttonAChanged(bool pressed)
{
    if (pressed) {
        auto *ui = m_mainWindow->ui;
        QWidget *currentWidget = ui->stackedViews->currentWidget();
        if (currentWidget == ui->pageFav) {
            m_mainWindow->loadSelectedPlayList();
        } else if (currentWidget == ui->pageFileSystem) {
            m_mainWindow->loadOrEnterSelectedPath();
        } else if (currentWidget == ui->pageGallery) {
            m_mainWindow->m_actToolBarImage->trigger();
        } else if (currentWidget == ui->pageImageView) {
            m_mainWindow->m_navigator->goNext();
        }
    }
}

void GamepadController::buttonBChanged(bool pressed)
{
    if (pressed) {
        auto *ui = m_mainWindow->ui;
        QWidget *currentWidget = ui->stackedViews->currentWidget();
        if (currentWidget == ui->pageImageView) {
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
    auto &secondaryViewModel = m_mainWindow->m_secondaryMainGraphicsViewModel;
    if (secondaryViewModel && secondaryViewModel->view()->isVisible()) {
        // Secondary view visible
        m_mainWindow->m_secondaryMainGraphicsViewModel->scroll(dx, dy);
    } else {
        m_mainWindow->m_mainGraphicsViewModel->scroll(dx, dy);
    }
}

void GamepadController::axisRightScroll(int dx, int dy)
{
    m_mainWindow->m_mainGraphicsViewModel->scroll(dx, dy);
}
