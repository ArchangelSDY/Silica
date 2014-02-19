#ifndef SQLITELOCALDATABASE_H
#define SQLITELOCALDATABASE_H

#include <QtSql>

#include "LocalDatabase.h"

class SQLiteLocalDatabase : public LocalDatabase
{
    Q_OBJECT
public:
    SQLiteLocalDatabase();

    QList<PlayListRecord> queryPlayListRecords();
    QStringList queryImageUrlsForPlayList(const QString &name);
    bool insertPlayListRecord(PlayListRecord *playListRecord);

private:
    void createTables();

    QSqlDatabase m_db;
};

#endif // SQLITELOCALDATABASE_H
