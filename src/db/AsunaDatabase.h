#ifndef ASUNADATABASE_H
#define ASUNADATABASE_H

#include <QSettings>

#include "RemoteDatabase.h"

class AsunaDatabase : public RemoteDatabase
{
public:
    virtual QUrl getUrlToQueryByTag(const QString &tag);

private:
    QSettings m_settings;
};

#endif // ASUNADATABASE_H
