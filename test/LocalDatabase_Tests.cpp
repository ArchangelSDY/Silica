#include <QtSql>
#include <QTest>

#include "../src/db/LocalDatabase.h"
#include "../src/image/ImageSource.h"
#include "../src/playlist/PlayListRecord.h"
#include "../src/GlobalConfig.h"
#include "../src/PlayList.h"

class TestLocalDatabase : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();

//    void localPlayListsSaveAndLoad();
//    void localPlayListsSaveAndLoad_data();
//    void playListRemove();
//    void playListUpdate();
//    void insertImagesToPlayList();
//    void removeImagesFromPlayList();

    void insertImage();
    void insertImage_data();

    void pluginPlayListProvider();
};

void TestLocalDatabase::initTestCase()
{
    Q_INIT_RESOURCE(silica);
    GlobalConfig::create();
    QVERIFY2(LocalDatabase::instance()->migrate(),
             "Fail to migrate database");

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "TestLocalDatabase");
    db.setDatabaseName("local.db");
    db.open();
    db.exec("delete from playlists");
    db.exec("delete from playlist_images");
    db.exec("delete from images");
    db.exec("delete from plugin_playlist_providers");
    db.close();
    QSqlDatabase::removeDatabase("TestLocalDatabase");
}

//void TestLocalDatabase::localPlayListsSaveAndLoad()
//{
//    QFETCH(QList<QUrl>, imageUrls);
//    QFETCH(QString, name);

//    PlayList pl(imageUrls);
//    LocalPlayListRecord record(name, pl[0]->thumbnailPath(), &pl);

//    int beforeCount = LocalDatabase::instance()->queryPlayListRecords().count();

//    bool ret = record.save();
//    QVERIFY(ret);

//    QList<PlayListRecord *> records = PlayListRecord::all();
//    QCOMPARE(records.count(), beforeCount + 1);

//    PlayListRecord *savedRecord = records[0];
//    QCOMPARE(savedRecord->name(), name);
//    QCOMPARE(savedRecord->coverPath(), pl[0]->thumbnailPath());
//    QCOMPARE(savedRecord->playList()->count(), pl.count());

//    while (!records.isEmpty()) {
//        delete records.takeFirst();
//    }
//}

//void TestLocalDatabase::localPlayListsSaveAndLoad_data()
//{
//    QTest::addColumn<QList<QUrl> >("imageUrls");
//    QTest::addColumn<QString>("name");
//    const QString &currentDir = qApp->applicationDirPath();

//    QTest::newRow("Basic")
//        << (QList<QUrl>() << QUrl("file://" + currentDir + "/assets/me.jpg"))
//        << QString("Fav");
//}

//void TestLocalDatabase::playListRemove()
//{
//    QList<QUrl> imageUrls;
//    imageUrls << QUrl("file:///me.jpg");
//    PlayList pl(imageUrls);
//    LocalPlayListRecord record("test_remove", "cover.jpg", &pl);
//    record.save();

//    QList<PlayListRecord *> recordsAfterSave = PlayListRecord::all();
//    QList<PlayListRecord *>::iterator it = recordsAfterSave.begin();
//    while (it != recordsAfterSave.end() &&  (*it)->name() != "test_remove") {
//        it++;
//    }

//    if (it == recordsAfterSave.end()) {
//        QFAIL("Cannot find saved playlist.");
//        return;
//    }

//    bool ok = (*it)->remove();
//    QCOMPARE(ok, true);

//    QList<PlayListRecord *> recordsAfterRemove = PlayListRecord::all();
//    it = recordsAfterRemove.begin();
//    while (it != recordsAfterRemove.end() && (*it)->name() != "test_remove") {
//        it++;
//    }

//    QCOMPARE(it, recordsAfterRemove.end());
//}

//void TestLocalDatabase::playListUpdate()
//{
//    QList<QUrl> imageUrls;
//    imageUrls << QUrl("file:///me.jpg");
//    PlayList pl(imageUrls);
//    LocalPlayListRecord record("test_update", "cover.jpg", &pl);
//    record.save();

//    QList<PlayListRecord *> recordsAfterSave = PlayListRecord::all();
//    QList<PlayListRecord *>::iterator it = recordsAfterSave.begin();
//    while (it != recordsAfterSave.end() &&  (*it)->name() != "test_update") {
//        it++;
//    }

//    if (it == recordsAfterSave.end()) {
//        QFAIL("Cannot find saved playlist.");
//        return;
//    }

//    PlayListRecord *savedRecord = *it;
//    savedRecord->setCoverPath("new_cover.jpg");
//    savedRecord->setName("test_update_new");
//    bool ok = savedRecord->save();

//    QCOMPARE(ok, true);

//    QList<PlayListRecord *> recordsAfterUpdate = PlayListRecord::all();
//    it = recordsAfterUpdate.begin();
//    while (it != recordsAfterUpdate.end() && (*it)->name() != "test_update_new") {
//        it++;
//    }

//    QCOMPARE((*it)->coverPath(), QString("new_cover.jpg"));
//}

//void TestLocalDatabase::insertImagesToPlayList()
//{
//    const QString &currentDir = qApp->applicationDirPath();
//    ImagePtr imageA(
//        new Image(QUrl("file://" + currentDir + "/assets/me.jpg")));
//    ImagePtr imageB(
//        new Image(QUrl("file://" + currentDir + "/assets/silica.png")));

//    PlayList pl;
//    pl << imageA;

//    LocalPlayListRecord record("test", "", &pl);
//    record.save();
//    int rid = record.id();

//    QVERIFY2(record.isSaved(), "PlayListRecord should be saved.");
//    QCOMPARE(record.playList()->count(), 1);

//    ImageList images;
//    images << imageB;
//    record.insertImages(images);

//    LocalPlayListRecord dupRecord("test", "");
//    qDebug() << "rid" << rid;
//    dupRecord.setId(rid);

//    PlayList *spl = dupRecord.playList();
//    QCOMPARE(spl->count(), 2);
//}

//void TestLocalDatabase::removeImagesFromPlayList()
//{
//    const QString &currentDir = qApp->applicationDirPath();
//    ImagePtr imageA(
//        new Image(QUrl("file://" + currentDir + "/assets/me.jpg")));
//    ImagePtr imageB(
//        new Image(QUrl("file://" + currentDir + "/assets/silica.png")));

//    PlayList pl;
//    pl << imageA;
//    pl << imageB;

//    LocalPlayListRecord record("test", "", &pl);
//    record.save();
//    int rid = record.id();

//    QVERIFY2(record.isSaved(), "PlayListRecord should be saved.");
//    QCOMPARE(record.playList()->count(), 2);

//    record.removeImage(imageB);

//    LocalPlayListRecord dupRecord("test", "");
//    qDebug() << "rid" << rid;
//    dupRecord.setId(rid);

//    PlayList *spl = dupRecord.playList();
//    QCOMPARE(spl->count(), 1);
//    QCOMPARE(spl->at(0)->source()->hashStr(), imageA->source()->hashStr());
//}

void TestLocalDatabase::insertImage()
{
    QFETCH(QUrl, imageUrl);

    Image image(imageUrl);

    bool ret = LocalDatabase::instance()->insertImage(&image);
    QVERIFY(ret);

    Image *insertedImage = LocalDatabase::instance()->queryImageByHashStr(
        image.source()->hashStr());
    QVERIFY(insertedImage != 0);

    QCOMPARE(insertedImage->name(), image.name());
    delete insertedImage;
}

void TestLocalDatabase::insertImage_data()
{
    QTest::addColumn<QUrl>("imageUrl");
    const QString &currentDir = qApp->applicationDirPath();

    QTest::newRow("Basic")
        << QUrl("file://" + currentDir + "/assets/insert_image.jpg");
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
