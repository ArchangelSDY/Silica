#ifndef TESTLOCALDATABASE_H
#define TESTLOCALDATABASE_H

#include <QObject>

class TestLocalDatabase : public QObject
{
    Q_OBJECT
private slots:
    void cleanup();

    void playListsSaveAndLoad();
    void playListsSaveAndLoad_data();

};

#endif // TESTLOCALDATABASE_H
