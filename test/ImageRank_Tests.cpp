#include <QtSql>
#include <QTest>

#include "../src/db/LocalDatabase.h"
#include "../src/image/Image.h"
#include "../src/image/ImageRank.h"

class TestImageRank : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanup();
    void saveAndLoad();
    void saveAndLoad_data();
    void upDownVote();
    void upDownVote_data();
};

void TestImageRank::initTestCase()
{
    Q_INIT_RESOURCE(silica);
    QVERIFY2(LocalDatabase::instance()->migrate(),
             "Fail to migrate database");
}

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

void TestImageRank::upDownVote()
{
    QFETCH(QUrl, imageUrl);
    QFETCH(int, oldRank);
    QFETCH(int, newRankAfterUpVote);
    QFETCH(int, newRankAfterDownVote);

    Image image(imageUrl);
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
    QTest::addColumn<QUrl>("imageUrl");
    QTest::addColumn<int>("oldRank");
    QTest::addColumn<int>("newRankAfterUpVote");
    QTest::addColumn<int>("newRankAfterDownVote");
    const QString &currentDir = qApp->applicationDirPath();
    QUrl imageUrl("file://" + currentDir + "/assets/me.jpg");

    QTest::newRow("Basic") << imageUrl << 3 << 4 << 2;
    QTest::newRow("Max rank")
        << imageUrl
        << ImageRank::MAX_VALUE
        << ImageRank::MAX_VALUE
        << ImageRank::MAX_VALUE - 1;
    QTest::newRow("Min rank")
        << imageUrl
        << ImageRank::MIN_VALUE
        << ImageRank::MIN_VALUE + 1
        << ImageRank::MIN_VALUE;
}

QTEST_MAIN(TestImageRank)
 #include "ImageRank_Tests.moc"
