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

    int queryImagesCount();
    bool insertImage(Image *image);

private:
    void createTablesIfNecessary();

    QSqlDatabase m_db;
};

#endif // SQLITELOCALDATABASE_H
