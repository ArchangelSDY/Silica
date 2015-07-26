#include "Logger.h"

#include <QMutex>

Logger *Logger::m_instance = 0;

Logger *Logger::instance()
{
    if (!m_instance) {
        QMutex mutex;
        mutex.lock();
        if (!m_instance) {
            m_instance = new Logger();
        }
        mutex.unlock();
    }

    return m_instance;
}

Logger::Logger()
{
}

void Logger::addListener(int type, LogListener *listener)
{
    m_listeners.insertMulti(type, listener);
}

LogBuilder Logger::log(int type)
{
    return LogBuilder(this, type);
}

void Logger::insert(const LogRecord &record)
{
    QList<LogListener *> listeners = m_listeners.values(record.type);
    foreach (LogListener *listener, listeners) {
        listener->dispatch(record);
    }
}
