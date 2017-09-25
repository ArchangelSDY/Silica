#include "GamepadAxisScroller.h"

const int GamepadAxisScroller::SCROLL_INTERVAL = 10;
const int GamepadAxisScroller::MAX_SCROLL_PIXELS_PER_INTERVAL = 50;

GamepadAxisScroller::GamepadAxisScroller()
{
    m_timer.setInterval(SCROLL_INTERVAL);
    connect(&m_timer, &QTimer::timeout, this, &GamepadAxisScroller::timeout);
}

void GamepadAxisScroller::setAxisXValue(double value)
{
    m_axisXValue = value;
    checkTimer();
}

void GamepadAxisScroller::setAxisYValue(double value)
{
    m_axisYValue = value;
    checkTimer();
}

void GamepadAxisScroller::checkTimer()
{
    if (m_timer.isActive() && qAbs(m_axisXValue) < 0.1 && qAbs(m_axisYValue) < 0.1) {
        m_timer.stop();
    } else if (!m_timer.isActive() && (qAbs(m_axisXValue) >= 0.1 || qAbs(m_axisYValue) >= 0.1)) {
        m_timer.start();
    }
}

void GamepadAxisScroller::timeout()
{
    emit scroll(- MAX_SCROLL_PIXELS_PER_INTERVAL * m_axisXValue, - MAX_SCROLL_PIXELS_PER_INTERVAL * m_axisYValue);
}