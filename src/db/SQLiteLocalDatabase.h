#ifndef SQLITELOCALDATABASE_H
#define SQLITELOCALDATABASE_H

#include "LocalDatabase.h"

class SQLiteLocalDatabase : public LocalDatabase
{
    Q_OBJECT
public:
    QList<PlayListRecord> playListRecords();
    bool savePlayListRecord(const PlayListRecord &playListRecord);

};

#endif // SQLITELOCALDATABASE_H
