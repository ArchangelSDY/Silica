#include "LocalDatabase.h"
#include "SQLiteLocalDatabase.h"

LocalDatabase *LocalDatabase::m_instance = 0;

LocalDatabase::LocalDatabase(QObject *parent) :
    QObject(parent)
{
}

LocalDatabase *LocalDatabase::instance()
{
    if (!m_instance) {
        m_instance = new SQLiteLocalDatabase();
    }

    return m_instance;
}
