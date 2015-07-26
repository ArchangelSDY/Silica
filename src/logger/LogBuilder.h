#ifndef LOGBUILDER_H
#define LOGBUILDER_H

#include <QString>
#include <QVariant>

#include "logger/LogRecord.h"

class Logger;

class LogBuilder
{
public:
    LogBuilder(Logger *logger, int type);

    LogBuilder &describe(const QString &key, const QVariant &value);
    void save();

private:
    Logger *m_logger;
    LogRecord m_record;
};

#endif // LOGBUILDER_H
