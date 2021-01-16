#include <QScopedPointer>
#include <QtSql>
#include <QTest>

#include "STestCase.h"
#include "../src/db/LocalDatabase.h"
#include "../src/image/ImageSource.h"
#include "../src/image/ImageSourceManager.h"
#include "../src/GlobalConfig.h"

class TestLocalDatabase : public STestCase
{
    Q_OBJECT
private slots:
    void init();

    void playListInsertQueryUpdateRemove();
    void localPlayListEntitiesInsertQueryRemove();

    void insertImage();
    void insertImage_data();
    void updateImageUrl();

    void pluginPlayListProvider();
};

void TestLocalDatabase::init()
{
    STestCase::initTestCase();

    QString dbConnName = QStringLiteral("%1_%2").arg(
        __func__, reinterpret_cast<uint64_t>(QThread::currentThreadId()));
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", dbConnName);
        db.setDatabaseName(GlobalConfig::instance()->localDatabasePath());
        db.open();
        db.exec("delete from playlists");
        db.exec("delete from playlist_images");
        db.exec("delete from images");
        db.exec("delete from plugin_playlist_providers");
    }
    QSqlDatabase::removeDatabase(dbConnName);
}

void TestLocalDatabase::playListInsertQueryUpdateRemove()
{
    int id = 0;

    // Insert
    {
        PlayListEntityData data;
        data.coverPath = "cover.png";
        data.name = "name";
        data.count = 1;
        QCOMPARE(data.id, 0);

        LocalDatabase::instance()->insertPlayListEntity(data);
        QVERIFY(data.id != 0);

        id = data.id;
    }

    // Query and update
    {
        auto allData = LocalDatabase::instance()->queryPlayListEntities(0);
        std::find_if(allData.constBegin(), allData.constEnd(), [id](auto d) { return d.id == id; });

        auto data = LocalDatabase::instance()->queryPlayListEntity(id);
        QVERIFY(data.isValid());
        QCOMPARE(data.id, id);
        QCOMPARE(data.coverPath, "cover.png");
        QCOMPARE(data.name, "name");
        QCOMPARE(data.count, 1);

        data.coverPath = "cover1.png";
        data.name = "name1";
        data.count = 2;
        LocalDatabase::instance()->updatePlayListEntity(data);
    }

    // Check update result
    {
        auto data = LocalDatabase::instance()->queryPlayListEntity(id);
        QVERIFY(data.isValid());
        QCOMPARE(data.id, id);
        QCOMPARE(data.coverPath, "cover1.png");
        QCOMPARE(data.name, "name1");
        QCOMPARE(data.count, 2);
    }

    // Remove
    {
        LocalDatabase::instance()->removePlayListEntity(id);
        auto data = LocalDatabase::instance()->queryPlayListEntity(id);
        QVERIFY(!data.isValid());
    }
}

void TestLocalDatabase::localPlayListEntitiesInsertQueryRemove()
{
    PlayListEntityData data;
    data.coverPath = "cover.png";
    data.name = "name";
    data.count = 0;
    LocalDatabase::instance()->insertPlayListEntity(data);

    // Initiallly there are no image urls
    {
        auto imageUrls = LocalDatabase::instance()->queryLocalPlayListEntityImageUrls(data.id);
        QCOMPARE(imageUrls.count(), 0);
    }

    // Insert some
    {
        QList<QUrl> imageUrls{ QUrl::fromLocalFile("c:/1.png"), QUrl::fromLocalFile("c:/2.png") };
        ImagePtr image1 = ImagePtr::create(imageUrls[0]);
        LocalDatabase::instance()->insertImage(image1.data());
        ImagePtr image2 = ImagePtr::create(imageUrls[1]);
        LocalDatabase::instance()->insertImage(image2.data());
        LocalDatabase::instance()->insertLocalPlayListEntityImageUrls(data.id, imageUrls);
    }

    // Query again
    {
        auto imageUrls = LocalDatabase::instance()->queryLocalPlayListEntityImageUrls(data.id);
        QCOMPARE(imageUrls.count(), 2);
        QCOMPARE(imageUrls[0].toLocalFile(), "c:/1.png");
        QCOMPARE(imageUrls[1].toLocalFile(), "c:/2.png");
    }

    // Remove some
    {
        QList<QUrl> imageUrls{ QUrl::fromLocalFile("c:/1.png") };
        LocalDatabase::instance()->removeLocalPlayListEntityImageUrls(data.id, imageUrls);
    }

    // Query again
    {
        auto imageUrls = LocalDatabase::instance()->queryLocalPlayListEntityImageUrls(data.id);
        QCOMPARE(imageUrls.count(), 1);
        QCOMPARE(imageUrls[0].toLocalFile(), "c:/2.png");
    }
}

void TestLocalDatabase::insertImage()
{
    QFETCH(QString, imagePath);

    Image image(imagePath);

    bool ret = LocalDatabase::instance()->insertImage(&image);
    QVERIFY(ret);

    QScopedPointer<Image> insertedImage(LocalDatabase::instance()->queryImageByHashStr(
        image.source()->hashStr()));
    QVERIFY(insertedImage != 0);

    QCOMPARE(insertedImage->name(), image.name());
}

void TestLocalDatabase::insertImage_data()
{
    QTest::addColumn<QString>("imagePath");

    QTest::newRow("Basic") << ":/assets/insert_image.jpg";
}

void TestLocalDatabase::updateImageUrl()
{
    QUrl oldUrl("file:///root/old/image.jpg");
    QUrl newUrl("file:///root/new/image.jpg");
    Image image(oldUrl);

    QVERIFY(LocalDatabase::instance()->insertImage(&image));

    QVERIFY(LocalDatabase::instance()->updateImageUrl(oldUrl, newUrl));

    Image *newImage = LocalDatabase::instance()->queryImageByHashStr(
        image.source()->hashStr());
    QVERIFY(newImage);

    QCOMPARE(newImage->source()->url(), newUrl);

    delete newImage;
}

void TestLocalDatabase::pluginPlayListProvider()
{
    QString pluginName = "com.silica.plugin";

    QCOMPARE(LocalDatabase::instance()->queryPluginPlayListProviderType(pluginName),
             -1);
    int typeId = LocalDatabase::instance()->insertPluginPlayListProviderType(pluginName);
    QCOMPARE(LocalDatabase::instance()->queryPluginPlayListProviderType(pluginName),
             typeId);
}

QTEST_MAIN(TestLocalDatabase)
#include "LocalDatabase_Tests.moc"
