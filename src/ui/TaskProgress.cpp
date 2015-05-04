#include "TaskProgress.h"

TaskProgress::TaskProgress(QObject *parent) : QObject(parent) ,
    m_minimum(0) ,
    m_maximum(0) ,
    m_value(0)
{
}

QString TaskProgress::key() const
{
    return m_key;
}

void TaskProgress::setKey(const QString &key)
{
    m_key = key;
}

bool TaskProgress::isRunning() const
{
    return m_isRunning;
}

void TaskProgress::start()
{
    m_isRunning = true;
    m_startTime = QDateTime::currentDateTime();
    emit changed();
}

void TaskProgress::stop()
{
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

void TaskProgress::reset()
{
    m_maximum = 0;
    m_minimum = 0;
    m_value = 0;
    m_isRunning = false;
    emit changed();
}
