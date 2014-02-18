#include "LocalDatabase.h"
#include "SQLiteLocalDatabase.h"

LocalDatabase *LocalDatabase::m_instance = new SQLiteLocalDatabase();

LocalDatabase::LocalDatabase(QObject *parent) :
    QObject(parent)
{
}
