#include "LogBuilder.h"

#include "logger/Logger.h"

LogBuilder::LogBuilder(Logger *logger, int type) : m_logger(logger)
{
    m_record.type = type;
}

LogBuilder &LogBuilder::describe(const QString &key, const QVariant &value)
{
    m_record.extra.insert(key, value);
    return *this;
}

void LogBuilder::save()
{
    m_logger->insert(m_record);
}
