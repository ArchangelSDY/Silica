#include <QTest>

#include "LocalDatabase.h"
#include "PlayList.h"

#include "TestLocalDatabase.h"

void TestLocalDatabase::playListsSaveAndLoad()
{
    QFETCH(QList<QUrl>, imageUrls);
    QFETCH(QString, name);

    PlayList pl(imageUrls);
    PlayListRecord record(name, pl[0]->thumbnailPath(), &pl);

    bool ret = LocalDatabase::instance()->savePlayListRecord(record);
    QVERIFY(ret);

    QList<PlayListRecord> records = LocalDatabase::instance()->playListRecords();
    QCOMPARE(records.count(), 1);

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

    QTest::newRow("Normal")
        << (QList<QUrl>() << QUrl("file:///" + currentDir + "/assets/me.jpg"))
        << QString("Fav");
}
