#ifndef TESTPLAYLIST_H
#define TESTPLAYLIST_H

#include <QObject>
#include <QTemporaryDir>

class TestPlayList : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();

    void sortByName();
    void sortByName_data();

private:
    QTemporaryDir m_tmpDir;
};

#endif // TESTPLAYLIST_H
