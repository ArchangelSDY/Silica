#include <QtSql>
#include <QTest>

#include "LocalDatabase.h"
#include "PlayList.h"
#include "PlayListRecord.h"

#include "TestLocalDatabase.h"

void TestLocalDatabase::cleanupTestCase()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "TestLocalDatabase");
    db.setDatabaseName("local.db");
    db.open();
    db.exec("delete from playlists");
    db.exec("delete from playlist_images");
    db.exec("delete from images");
    db.close();
    QSqlDatabase::removeDatabase("TestLocalDatabase");
}

void TestLocalDatabase::playListsSaveAndLoad()
{
    QFETCH(QList<QUrl>, imageUrls);
    QFETCH(QString, name);

    PlayList pl(imageUrls);
    PlayListRecord record(name, pl[0]->thumbnailPath(), &pl);

    int beforeCount = LocalDatabase::instance()->queryPlayListRecords().count();

    bool ret = record.saveToLocalDatabase();
    QVERIFY(ret);

    QList<PlayListRecord> records = PlayListRecord::fromLocalDatabase();
    QCOMPARE(records.count(), beforeCount + 1);

    PlayListRecord &savedRecord = records[0];
    QCOMPARE(savedRecord.name(), name);
    QCOMPARE(savedRecord.coverPath(), pl[0]->thumbnailPath());
    QCOMPARE(savedRecord.playList()->count(), pl.count());
}

void TestLocalDatabase::playListsSaveAndLoad_data()
{
    QTest::addColumn<QList<QUrl> >("imageUrls");
    QTest::addColumn<QString>("name");
    const QString &currentDir = qApp->applicationDirPath();

    QTest::newRow("Basic")
        << (QList<QUrl>() << QUrl("file://" + currentDir + "/assets/me.jpg"))
        << QString("Fav");
}

void TestLocalDatabase::insertImage()
{
    QFETCH(QUrl, imageUrl);
    QFETCH(int, deltaCount);

    Image image(imageUrl);

    int beforeCount = LocalDatabase::instance()->queryImagesCount();
    bool ret = LocalDatabase::instance()->insertImage(&image);
    QVERIFY(ret);
    int afterCount = LocalDatabase::instance()->queryImagesCount();

    QCOMPARE(afterCount - beforeCount, deltaCount);
}

void TestLocalDatabase::insertImage_data()
{
    QTest::addColumn<QUrl>("imageUrl");
    QTest::addColumn<int>("deltaCount");
    const QString &currentDir = qApp->applicationDirPath();

    QTest::newRow("Basic")
        << QUrl("file://" + currentDir + "/assets/me.jpg")
        << 1;
    QTest::newRow("Duplicate")
        << QUrl("file://" + currentDir + "/assets/me.jpg")
        << 0;
}
