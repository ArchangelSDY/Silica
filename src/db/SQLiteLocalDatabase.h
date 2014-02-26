#ifndef SQLITELOCALDATABASE_H
#define SQLITELOCALDATABASE_H

#include <QtSql>

#include "Image.h"
#include "LocalDatabase.h"

class SQLiteLocalDatabase : public LocalDatabase
{
    Q_OBJECT
public:
    SQLiteLocalDatabase();

    QList<PlayListRecord> queryPlayListRecords();
    QStringList queryImageUrlsForPlayList(int playListId);
    bool insertPlayListRecord(PlayListRecord *playListRecord);

    int queryImagesCount();
    bool insertImage(Image *image);
    Image *queryImageByHashStr(const QString &hashStr);

private:
    void createTablesIfNecessary();

    QSqlDatabase m_db;
};

#endif // SQLITELOCALDATABASE_H
