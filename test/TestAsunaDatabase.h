#ifndef TESTASUNADATABASE_H
#define TESTASUNADATABASE_H

#include <QObject>

#include "AsunaDatabase.h"
#include "deps/QtMockWebServer/src/QtMockWebServer.h"

class TestAsunaDatabase : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();
    void addImageToAlbum();
    void addImageToAlbum_data();

private:
    AsunaDatabase *m_database;
    QtMockWebServer *m_mockServer;
};

#endif // TESTASUNADATABASE_H
