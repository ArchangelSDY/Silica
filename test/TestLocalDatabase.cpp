#include <QTest>

#include "PlayList.h"
#include "PlayListRecord.h"

#include "TestLocalDatabase.h"

void TestLocalDatabase::cleanup()
{
    QFile f("local.db");
    if (f.exists()) {
        f.remove();
    }
}

void TestLocalDatabase::playListsSaveAndLoad()
{
    QFETCH(QList<QUrl>, imageUrls);
    QFETCH(QString, name);

    PlayList pl(imageUrls);
    PlayListRecord record(name, pl[0]->thumbnailPath(), &pl);

    bool ret = record.saveToLocalDatabase();
    QVERIFY(ret);

    QList<PlayListRecord> records = PlayListRecord::fromLocalDatabase();
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
        << (QList<QUrl>() << QUrl("file://" + currentDir + "/assets/me.jpg"))
        << QString("Fav");
}
