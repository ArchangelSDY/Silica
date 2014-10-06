#include "AsunaDatabase.h"
#include "RemoteDatabase.h"

RemoteDatabase *RemoteDatabase::s_instance = 0;

RemoteDatabase *RemoteDatabase::instance()
{
    if (!s_instance) {
        s_instance = new AsunaDatabase();
    }

    return s_instance;
}

RemoteDatabase::RemoteDatabase()
{
}
