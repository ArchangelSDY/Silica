#ifndef TESTLOCALDATABASE_H
#define TESTLOCALDATABASE_H

#include <QObject>

class TestLocalDatabase : public QObject
{
    Q_OBJECT
private slots:
    void cleanupTestCase();

    void playListsSaveAndLoad();
    void playListsSaveAndLoad_data();

    void insertImage();
    void insertImage_data();
};

#endif // TESTLOCALDATABASE_H
