#include <QSignalSpy>
#include <QtSql>
#include <QTest>

#include "../src/GlobalConfig.h"
#include "../src/db/LocalDatabase.h"
#include "../src/image/Image.h"
#include "../src/image/ImageSource.h"

class TestImage : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void init();
    void cleanup();
    void loadThumbnail();
    void loadThumbnail_data();
    void hotspots();
    void hotspots_data();
};

void TestImage::initTestCase()
{
    Q_INIT_RESOURCE(silica);
    GlobalConfig::create();
    QVERIFY2(LocalDatabase::instance()->migrate(),
             "Fail to migrate database");
}

void TestImage::init()
{
    QDir d("thumbnails");
    if (d.exists()) {
        d.removeRecursively();
    }
}

void TestImage::cleanup()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "TestImage");
    db.setDatabaseName("local.db");
    db.open();
    db.exec("delete from images");
    db.exec("delete from image_hotspots");
    db.close();
    QSqlDatabase::removeDatabase("TestImage");
}

void TestImage::loadThumbnail()
{
    QFETCH(QUrl, imageUrl);

    Image image(imageUrl);
    QSignalSpy spyLoad(&image, SIGNAL(thumbnailLoaded()));
    image.loadThumbnail(true);
    spyLoad.wait();

    QVERIFY(!image.thumbnail().isNull());
    QVERIFY(image.thumbnail().width() > 0);

    Image *insertedImage = LocalDatabase::instance()->queryImageByHashStr(
        image.source()->hashStr());
    QVERIFY(insertedImage !=  0);
    QCOMPARE(insertedImage->name(), image.name());
    delete insertedImage;
}

void TestImage::loadThumbnail_data()
{
    QTest::addColumn<QUrl>("imageUrl");
    const QString &currentDir = qApp->applicationDirPath();

    QTest::newRow("Basic")
        << QUrl("file://" + currentDir + "/assets/me.jpg");
}

void TestImage::hotspots()
{
    QFETCH(QUrl, imageUrl);
    QFETCH(QList<QRect>, rects);

    Image image(imageUrl);
    foreach (const QRect &rect, rects) {
        ImageHotspot hotspot(&image, rect);
        hotspot.save();
    }

    QList<ImageHotspot *> hotspots = image.hotspots();
    QCOMPARE(hotspots.count(), rects.count());
    foreach (ImageHotspot *hotspot, hotspots) {
        QCOMPARE(rects.contains(hotspot->rect()), true);
    }
}

void TestImage::hotspots_data()
{
    QTest::addColumn<QUrl>("imageUrl");
    QTest::addColumn<QList<QRect> >("rects");
    const QString &currentDir = qApp->applicationDirPath();

    QTest::newRow("Basic")
        << QUrl("file://" + currentDir + "/assets/me.jpg")
        << (QList<QRect>() << QRect(0, 0, 10, 10) << QRect(10, 10, 20, 20));
}

QTEST_MAIN(TestImage)
 #include "Image_Tests.moc"
