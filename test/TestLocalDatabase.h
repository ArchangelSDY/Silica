#ifndef TESTLOCALDATABASE_H
#define TESTLOCALDATABASE_H

#include <QObject>

class TestLocalDatabase : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();

    void localPlayListsSaveAndLoad();
    void localPlayListsSaveAndLoad_data();
    void playListRemove();
    void playListUpdate();
    void insertImagesToPlayList();
    void removeImagesFromPlayList();

    void insertImage();
    void insertImage_data();
};

#endif // TESTLOCALDATABASE_H
