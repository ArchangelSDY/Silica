#include <QtSql>
#include <QTest>

#include "Image.h"
#include "ImageRank.h"
#include "LocalDatabase.h"

#include "TestImageRank.h"

void TestImageRank::cleanup()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "TestImageRank");
    db.setDatabaseName("local.db");
    db.open();
    db.exec("delete from images");
    db.exec("delete from image_ranks");
    db.close();
    QSqlDatabase::removeDatabase("TestImageRank");
}

void TestImageRank::saveAndLoad()
{
    QFETCH(QUrl, imageUrl);
    QFETCH(int, newRank);

    Image image(imageUrl);

    int defaultRank = image.rank()->value();
    QCOMPARE(defaultRank, ImageRank::DEFAULT_VALUE);

    image.rank()->setValue(newRank);
    QCOMPARE(image.rank()->value(), newRank);

    int savedValue = LocalDatabase::instance()->queryImageRankValue(&image);
    QCOMPARE(savedValue, newRank);
}

void TestImageRank::saveAndLoad_data()
{
    QTest::addColumn<QUrl>("imageUrl");
    QTest::addColumn<int>("newRank");
    const QString &currentDir = qApp->applicationDirPath();

    QTest::newRow("Basic")
        << QUrl("file://" + currentDir + "/assets/me.jpg")
        << 5;
}
