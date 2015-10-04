#include "TaskProgress.h"

#include "db/LocalDatabase.h"

TaskProgress::TaskProgress(QObject *parent) : QObject(parent) ,
    m_minimum(0) ,
    m_maximum(0) ,
    m_value(0) ,
    m_isRunning(false) ,
    m_lastTimeConsumption(0) ,
    m_enableEstimate(false)
{
    m_estimateTimer.setInterval(100);
    m_estimateTimer.setSingleShot(false);
    connect(&m_estimateTimer, SIGNAL(timeout()),
            this, SLOT(estimateUpdate()));
}

QString TaskProgress::key() const
{
    return m_key;
}

void TaskProgress::setKey(const QString &key)
{
    m_key = key;
    emit changed();
}

bool TaskProgress::isRunning() const
{
    return m_isRunning;
}

void TaskProgress::start()
{
    m_isRunning = true;
    m_startTime = QDateTime::currentDateTime();
    if (m_enableEstimate) {
        m_lastTimeConsumption =
            LocalDatabase::instance()->queryTaskProgressTimeConsumption(m_key);

        m_estimateTimer.start();
    }
    emit changed();
}

void TaskProgress::stop()
{
    m_isRunning = false;
    m_estimateTimer.stop();

    m_lastTimeConsumption = m_startTime.msecsTo(QDateTime::currentDateTime());
    if (m_enableEstimate) {
        LocalDatabase::instance()->saveTaskProgressTimeConsumption(
            m_key, m_lastTimeConsumption);
    }

    emit changed();
}

void TaskProgress::reset()
{
    m_value = 0;
    m_isRunning = false;
    emit changed();
}

int TaskProgress::maximum() const
{
    return m_maximum;
}

void TaskProgress::setMaximum(int max)
{
    m_maximum = max;
    emit changed();
}

int TaskProgress::minimum() const
{
    return m_minimum;
}

void TaskProgress::setMinimum(int min)
{
    m_minimum = min;
    emit changed();
}

int TaskProgress::value() const
{
    return m_value;
}

void TaskProgress::setValue(int val)
{
    if (val < m_minimum || val > m_maximum) {
        return;
    }
    m_value = val;
    emit changed();
}

void TaskProgress::setEstimateEnabled(bool enabled)
{
    m_enableEstimate = enabled;
    if (enabled && m_isRunning && !m_estimateTimer.isActive()) {
        m_estimateTimer.start();
    }
    if (!enabled) {
        m_estimateTimer.stop();
    }
}

void TaskProgress::setEstimateInterval(int interval)
{
    m_estimateTimer.setInterval(interval);
}

void TaskProgress::estimateUpdate()
{
    double progress;
    if (m_lastTimeConsumption > 0) {
        // Estimate current progress based on last record
        qint64 curConsumption = m_startTime.msecsTo(QDateTime::currentDateTime());
        progress = (double) curConsumption / m_lastTimeConsumption;
    } else {
        // No last record, add 1% per interval
        progress += 0.01;
    }

    // Stop at 90%
    progress = progress > 0.9 ? 0.9 : progress;

    setValue((m_maximum - m_minimum) * progress + m_minimum);
}
