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
    QFETCH(int, setRank);
    QFETCH(int, savedRank);

    Image image(imageUrl);

    int defaultRank = image.rank()->value();
    QCOMPARE(defaultRank, ImageRank::DEFAULT_VALUE);

    image.rank()->setValue(setRank);
    QCOMPARE(image.rank()->value(), savedRank);

    int savedValue = LocalDatabase::instance()->queryImageRankValue(&image);
    QCOMPARE(savedValue, savedRank);
}

void TestImageRank::saveAndLoad_data()
{
    QTest::addColumn<QUrl>("imageUrl");
    QTest::addColumn<int>("setRank");
    QTest::addColumn<int>("savedRank");
    const QString &currentDir = qApp->applicationDirPath();
    QUrl imageUrl("file://" + currentDir + "/assets/me.jpg");

    QTest::newRow("Basic") << imageUrl << 5 << 5;
    QTest::newRow("Too large rank") << imageUrl << 10 << ImageRank::MAX_VALUE;
    QTest::newRow("Too small rank") << imageUrl << 0 << ImageRank::MIN_VALUE;
}
