#ifndef ASUNADATABASE_H
#define ASUNADATABASE_H

#include "Database.h"

class AsunaDatabase : public Database
{
public:
    AsunaDatabase();

    virtual void queryByTag(const QString &tag, int page = 1);

public slots:
    virtual void reqFinish(QNetworkReply *);

private:
    void parseJsonResponse(QNetworkReply *);
    void queryNextPage(QUrl url);
};

#endif // ASUNADATABASE_H
