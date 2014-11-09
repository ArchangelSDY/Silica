#ifndef TESTASUNADATABASE_H
#define TESTASUNADATABASE_H

#include <QObject>

class AsunaDatabase;
class QtMockWebServer;

class TestAsunaDatabase : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();
    void queryImagesByTag();
    void queryImagesByTag_data();
    void addImageToAlbum();
    void addImageToAlbum_data();

private:
    AsunaDatabase *m_database;
    QtMockWebServer *m_mockServer;
};

#endif // TESTASUNADATABASE_H
