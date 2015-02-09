#include <QtSql>
#include <QTest>

#include "STestCase.h"
#include "../src/GlobalConfig.h"
#include "../src/db/LocalDatabase.h"
#include "../src/image/Image.h"
#include "../src/image/ImageRank.h"

class TestImageRank : public STestCase
{
    Q_OBJECT
private slots:
    void cleanup();
    void saveAndLoad();
    void saveAndLoad_data();
    void upDownVote();
    void upDownVote_data();
};

void TestImageRank::cleanup()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "TestImageRank");
    db.setDatabaseName(GlobalConfig::instance()->localDatabasePath());
    db.open();
    db.exec("delete from images");
    db.exec("delete from image_ranks");
    db.close();
    QSqlDatabase::removeDatabase("TestImageRank");
}

void TestImageRank::saveAndLoad()
{
    QFETCH(QString, imagePath);
    QFETCH(int, setRank);
    QFETCH(int, savedRank);

    Image image(imagePath);

    int defaultRank = image.rank()->value();
    QCOMPARE(defaultRank, ImageRank::DEFAULT_VALUE);

    image.rank()->setValue(setRank);
    QCOMPARE(image.rank()->value(), savedRank);

    int savedValue = LocalDatabase::instance()->queryImageRankValue(&image);
    QCOMPARE(savedValue, savedRank);
}

void TestImageRank::saveAndLoad_data()
{
    QTest::addColumn<QString>("imagePath");
    QTest::addColumn<int>("setRank");
    QTest::addColumn<int>("savedRank");
    QString imagePath = ":/assets/me.jpg";

    QTest::newRow("Basic") << imagePath << 5 << 5;
    QTest::newRow("Too large rank") << imagePath << 10 << ImageRank::MAX_VALUE;
    QTest::newRow("Too small rank") << imagePath << 0 << ImageRank::MIN_VALUE;
}

void TestImageRank::upDownVote()
{
    QFETCH(QString, imagePath);
    QFETCH(int, oldRank);
    QFETCH(int, newRankAfterUpVote);
    QFETCH(int, newRankAfterDownVote);

    Image image(imagePath);
    image.rank()->setValue(oldRank);

    // Test up vote
    image.rank()->upVote();
    QCOMPARE(image.rank()->value(), newRankAfterUpVote);
    QCOMPARE(
        LocalDatabase::instance()->queryImageRankValue(&image),
        newRankAfterUpVote);

    // Reset rank
    image.rank()->setValue(oldRank);

    // Test down vote
    image.rank()->downVote();
    QCOMPARE(image.rank()->value(), newRankAfterDownVote);
    QCOMPARE(
        LocalDatabase::instance()->queryImageRankValue(&image),
        newRankAfterDownVote);
}

void TestImageRank::upDownVote_data()
{
    QTest::addColumn<QString>("imagePath");
    QTest::addColumn<int>("oldRank");
    QTest::addColumn<int>("newRankAfterUpVote");
    QTest::addColumn<int>("newRankAfterDownVote");
    QString imagePath = ":/assets/me.jpg";

    QTest::newRow("Basic") << imagePath << 3 << 4 << 2;
    QTest::newRow("Max rank")
        << imagePath
        << ImageRank::MAX_VALUE
        << ImageRank::MAX_VALUE
        << ImageRank::MAX_VALUE - 1;
    QTest::newRow("Min rank")
        << imagePath
        << ImageRank::MIN_VALUE
        << ImageRank::MIN_VALUE + 1
        << ImageRank::MIN_VALUE;
}

QTEST_MAIN(TestImageRank)
 #include "ImageRank_Tests.moc"
