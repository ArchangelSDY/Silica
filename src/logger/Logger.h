#ifndef LOGGER_H
#define LOGGER_H

#include <QHash>
#include <QList>

#include "logger/LogBuilder.h"
#include "logger/LogRecord.h"

class Logger
{
public:
    class LogListener {
    public:
        virtual void dispatch(const LogRecord &record) = 0;
    };

    static const int IMAGE_LOAD_ERROR = 1;

    static Logger *instance();

    void addListener(int type, LogListener *listener);
    LogBuilder log(int type);

private:
    Logger();

    friend class LogBuilder;
    void insert(const LogRecord &record);

    static Logger *m_instance;
    QHash<int, LogListener *> m_listeners;
};

#endif // LOGGER_H
