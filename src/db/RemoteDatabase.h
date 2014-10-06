#ifndef REMOTEDATABASE_H
#define REMOTEDATABASE_H

#include <QUrl>

class RemoteDatabase
{
public:
    static RemoteDatabase *instance();

    virtual QUrl getUrlToQueryByTag(const QString &tag) = 0;

protected:
    RemoteDatabase();

private:
    static RemoteDatabase *s_instance;
};

#endif // REMOTEDATABASE_H
