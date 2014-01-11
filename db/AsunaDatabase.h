#ifndef ASUNADATABASE_H
#define ASUNADATABASE_H

#include <QSettings>

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

    QSettings m_settings;
};

#endif // ASUNADATABASE_H
