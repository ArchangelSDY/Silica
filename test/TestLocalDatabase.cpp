#include <QtSql>
#include <QTest>

#include "LocalDatabase.h"
#include "PlayList.h"
#include "PlayListRecord.h"

#include "TestLocalDatabase.h"

void TestLocalDatabase::initTestCase()
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

    bool ret = record.save();
    QVERIFY(ret);

    QList<PlayListRecord> records = PlayListRecord::all();
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

void TestLocalDatabase::playListRemove()
{
    QList<QUrl> imageUrls;
    imageUrls << QUrl("file:///me.jpg");
    PlayList pl(imageUrls);
    PlayListRecord record("test_remove", "cover.jpg", &pl);
    record.save();

    QList<PlayListRecord> recordsAfterSave = PlayListRecord::all();
    QList<PlayListRecord>::iterator it = recordsAfterSave.begin();
    while (it != recordsAfterSave.end() &&  it->name() != "test_remove") {
        it++;
    }

    if (it == recordsAfterSave.end()) {
        QFAIL("Cannot find saved playlist.");
        return;
    }

    bool ok = it->remove();
    QCOMPARE(ok, true);

    QList<PlayListRecord> recordsAfterRemove = PlayListRecord::all();
    it = recordsAfterRemove.begin();
    while (it != recordsAfterRemove.end() && it->name() != "test_remove") {
        it++;
    }

    QCOMPARE(it, recordsAfterRemove.end());
}

void TestLocalDatabase::insertImage()
{
    QFETCH(QUrl, imageUrl);

    Image image(imageUrl);

    bool ret = LocalDatabase::instance()->insertImage(&image);
    QVERIFY(ret);

    Image *insertedImage = LocalDatabase::instance()->queryImageByHashStr(
        image.source()->hashStr());
    QVERIFY(insertedImage != 0);

    QCOMPARE(insertedImage->name(), image.name());
    delete insertedImage;
}

void TestLocalDatabase::insertImage_data()
{
    QTest::addColumn<QUrl>("imageUrl");
    const QString &currentDir = qApp->applicationDirPath();

    QTest::newRow("Basic")
        << QUrl("file://" + currentDir + "/assets/insert_image.jpg");
}
