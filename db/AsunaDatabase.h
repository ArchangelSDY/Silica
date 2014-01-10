#ifndef ASUNADATABASE_H
#define ASUNADATABASE_H

#include "Database.h"

class AsunaDatabase : public Database
{
public:
    AsunaDatabase();

    virtual void queryByTag(const QString &tag);

public slots:
    virtual void reqFinish(QNetworkReply *);
};

#endif // ASUNADATABASE_H
