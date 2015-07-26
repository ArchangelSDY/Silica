#ifndef LOGRECORD_H
#define LOGRECORD_H

#include <QHash>
#include <QVariant>

class LogRecord
{
public:
    int type;
    QHash<QString, QVariant> extra;
};

#endif // LOGRECORD_H
