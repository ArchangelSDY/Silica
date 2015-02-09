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

void TestImage::loadThumbnail()
{
    QFETCH(QString, imagePath);

    Image image(imagePath);
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
