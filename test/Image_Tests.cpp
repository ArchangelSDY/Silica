#include <QSignalSpy>
#include <QtSql>
#include <QTest>

#include "STestCase.h"
#include "../src/GlobalConfig.h"
#include "../src/db/LocalDatabase.h"
#include "../src/image/Image.h"
#include "../src/image/ImageSource.h"

class TestImage : public STestCase
{
    Q_OBJECT
private slots:
    void cleanup();
    void load();
    void load_data();
    void loadThumbnail();
    void loadThumbnail_data();
    void hotspots();
    void hotspots_data();
};

void TestImage::cleanup()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "TestImage");
    db.setDatabaseName(GlobalConfig::instance()->localDatabasePath());
    db.open();
    db.exec("delete from images");
    db.exec("delete from image_hotspots");
    db.close();
    QSqlDatabase::removeDatabase("TestImage");
}

void TestImage::load()
{
    QFETCH(QString, imagePath);
    QFETCH(bool, isSuccess);
    QFETCH(int, loadCnt);

    Image image(imagePath);
    QCOMPARE(image.status(), Image::NotLoad);
    QVERIFY2(image.data().isNull(), "Image data should be null before load");

    QSignalSpy spyLoad(&image, SIGNAL(loaded()));
    for (int i = 0; i < loadCnt; ++i) {
        image.load();
    }
    QCOMPARE(image.status(), Image::Loading);
    QVERIFY(spyLoad.wait());

    QCOMPARE(image.status(),
             isSuccess ? Image::LoadComplete : Image::LoadError);
    QCOMPARE(!image.data().isNull(), isSuccess);

    for (int i = 0; i < loadCnt - 1; ++i) {
        image.scheduleUnload();
        QVERIFY2(!image.data().isNull(),
                 "Image should not unload until last load request finished");
    }
    image.scheduleUnload();
    QCOMPARE(image.status(), Image::NotLoad);
    QVERIFY2(image.data().isNull(), "Image data should be null after unload");
}

void TestImage::load_data()
{
    QTest::addColumn<QString>("imagePath");
    QTest::addColumn<bool>("isSuccess");
    QTest::addColumn<int>("loadCnt");

    QTest::newRow("Load successfully")
        << ":/assets/silica.png"
        << true
        << 1;
    QTest::newRow("Multiple loads")
        << ":/assets/silica.png"
        << true
        << 3;
}

void TestImage::loadThumbnail()
{
    QFETCH(QString, imagePath);

    Image image(imagePath);
    QSignalSpy spyLoad(&image, &Image::thumbnailLoaded);
    image.loadThumbnail(true);
    QVERIFY(spyLoad.wait());

    QVERIFY(!image.thumbnail().isNull());
    QVERIFY(image.thumbnail()->width() > 0);
    QVERIFY2(image.data().isNull(), "Image should unload after thumbnail made");

    Image *insertedImage = LocalDatabase::instance()->queryImageByHashStr(
        image.source()->hashStr());
    QVERIFY(insertedImage !=  0);
    QCOMPARE(insertedImage->name(), image.name());
    delete insertedImage;
}

void TestImage::loadThumbnail_data()
{
    QTest::addColumn<QString>("imagePath");

    QTest::newRow("Basic")
        << ":/assets/me.jpg";
}

void TestImage::hotspots()
{
    QFETCH(QString, imagePath);
    QFETCH(QList<QRect>, rects);

    Image image(imagePath);
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
    QTest::addColumn<QString>("imagePath");
    QTest::addColumn<QList<QRect> >("rects");

    QTest::newRow("Basic")
        << ":/assets/me.jpg"
        << (QList<QRect>() << QRect(0, 0, 10, 10) << QRect(10, 10, 20, 20));
}

QTEST_MAIN(TestImage)
 #include "Image_Tests.moc"
