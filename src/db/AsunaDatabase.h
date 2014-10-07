#ifndef ASUNADATABASE_H
#define ASUNADATABASE_H

#include <QSettings>

#include "RemoteDatabase.h"

class AsunaDatabase : public RemoteDatabase
{
public:
    virtual MultiPageReplyIterator *queryImagesByTag(const QString &tag);

private:
    QSettings m_settings;
};

#endif // ASUNADATABASE_H
