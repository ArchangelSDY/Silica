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
};

Q_DECLARE_METATYPE(QSharedPointer<QImage>)
Q_DECLARE_METATYPE(QSharedPointer<ImageData>)

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

    Image image(imagePath);
    QSharedPointer<ImageData> imageData;

    // First load
    {
        QSignalSpy spyLoad(&image, &Image::loaded);
        image.load();
        QVERIFY(image.isLoading());
        QVERIFY(spyLoad.wait());

        QVERIFY(!image.isLoading());
        QCOMPARE(image.isError(), !isSuccess);
        QVERIFY2(!image.data().isNull(), "Image data should not be null as data is owned by spyLoad");
        QCOMPARE(image.data().toStrongRef()->defaultFrame().isNull(), !isSuccess);

        QCOMPARE(spyLoad.count(), 1);
        QList<QVariant> args = spyLoad.takeFirst();
        QCOMPARE(args.count(), 1);
        imageData = args.takeFirst().value<QSharedPointer<ImageData>>();
        QVERIFY(!imageData.isNull());
        QCOMPARE(imageData->defaultFrame().isNull(), !isSuccess);
    }

    // Second load should return immediately as we cache a weak reference
    {
        QSignalSpy spyLoad(&image, &Image::loaded);
        QCOMPARE(spyLoad.count(), 0);
        image.load();
        QCOMPARE(spyLoad.count(), 1);
        auto imageData2 = spyLoad.takeFirst().takeFirst().value<QSharedPointer<ImageData>>();
        QVERIFY(!imageData2.isNull());
        QCOMPARE(imageData2->defaultFrame().isNull(), !isSuccess);
    }

    // Release all references
    imageData.reset();
    QVERIFY2(image.data().isNull(), "Image data should be null as data is not owned by anyone");
}

void TestImage::load_data()
{
    QTest::addColumn<QString>("imagePath");
    QTest::addColumn<bool>("isSuccess");

    QTest::newRow("Load successfully")
        << ":/assets/silica.png"
        << true;
}

void TestImage::loadThumbnail()
{
    QFETCH(QString, imagePath);

    Image image(imagePath);
    QSignalSpy spyLoad(&image, &Image::thumbnailLoaded);
    image.loadThumbnail();
    QVERIFY(spyLoad.wait());

    QList<QVariant> arguments = spyLoad.takeFirst();
    QSharedPointer<QImage> thumbnail = arguments.at(0).value<QSharedPointer<QImage>>();
    QVERIFY(!thumbnail.isNull());
    QVERIFY(thumbnail->width() > 0);
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

QTEST_MAIN(TestImage)
 #include "Image_Tests.moc"
