#include <QSignalSpy>
#include <QtSql>
#include <QTest>

#include "LocalDatabase.h"
#include "Image.h"

#include "TestImage.h"

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
    db.exec("delete from images;");
    db.close();
    QSqlDatabase::removeDatabase("test");
}

void TestImage::loadThumbnail()
{
    QFETCH(QUrl, imageUrl);

    int beforeImagesCount = LocalDatabase::instance()->queryImagesCount();

    Image image(imageUrl);
    QSignalSpy spyLoad(&image, SIGNAL(thumbnailLoaded()));
    image.loadThumbnail(true);
    spyLoad.wait();

    QVERIFY(!image.thumbnail().isNull());
    QVERIFY(image.thumbnail().width() > 0);

    int afterImagesCount = LocalDatabase::instance()->queryImagesCount();
    QCOMPARE(afterImagesCount, beforeImagesCount + 1);
}

void TestImage::loadThumbnail_data()
{
    QTest::addColumn<QUrl>("imageUrl");
    const QString &currentDir = qApp->applicationDirPath();

    QTest::newRow("Basic")
        << QUrl("file://" + currentDir + "/assets/me.jpg");
}
