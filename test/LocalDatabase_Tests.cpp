#include <QtSql>
#include <QTest>

#include "STestCase.h"
#include "../src/db/LocalDatabase.h"
#include "../src/image/ImageSource.h"
#include "../src/image/ImageSourceManager.h"
#include "../src/playlist/LocalPlayListProvider.h"
#include "../src/playlist/LocalPlayListProviderFactory.h"
#include "../src/playlist/PlayListRecord.h"
#include "../src/GlobalConfig.h"
#include "../src/PlayList.h"

class TestLocalDatabase : public STestCase
{
    Q_OBJECT
private slots:
    void initTestCase() override;

    void localPlayListsSaveAndLoad();
    void localPlayListsSaveAndLoad_data();
    void playListRemove();
    void playListUpdate();
    void insertImagesToPlayList();
    void removeImagesFromPlayList();

    void insertImage();
    void insertImage_data();
    void updateImageUrl();

    void pluginPlayListProvider();
};

void TestLocalDatabase::initTestCase()
{
    STestCase::initTestCase();

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "TestLocalDatabase");
    db.setDatabaseName(GlobalConfig::instance()->localDatabasePath());
    db.open();
    db.exec("delete from playlists");
    db.exec("delete from playlist_images");
    db.exec("delete from images");
    db.exec("delete from plugin_playlist_providers");
    db.close();
    QSqlDatabase::removeDatabase("TestLocalDatabase");
}

void TestLocalDatabase::localPlayListsSaveAndLoad()
{
    QFETCH(QList<QString>, imagePaths);
    QFETCH(QString, name);

    PlayList pl;
    foreach (const QString &path, imagePaths) {
        pl.addSinglePath(path);
    }

    LocalPlayListProvider provider;
    PlayListRecordBuilder plrBuilder;
    plrBuilder
        .setName(name)
        .setCoverPath("/cover/path.png")
        .setPlayList(&pl)
        .setType(LocalPlayListProviderFactory::TYPE);
    PlayListRecord *record = plrBuilder.obtain();

    int beforeCount = LocalDatabase::instance()->queryPlayListRecords().count();

    bool ret = record->save();
    QVERIFY(ret);

    QList<PlayListRecord *> records = PlayListRecord::all();
    QCOMPARE(records.count(), beforeCount + 1);

    PlayListRecord *savedRecord = records[0];
    QCOMPARE(savedRecord->name(), name);
    QCOMPARE(savedRecord->coverPath(), QString("/cover/path.png"));
    QCOMPARE(savedRecord->playList()->count(), pl.count());

    qDeleteAll(records.begin(), records.end());
    delete record;
}

void TestLocalDatabase::localPlayListsSaveAndLoad_data()
{
    QTest::addColumn<QList<QString> >("imagePaths");
    QTest::addColumn<QString>("name");

    QTest::newRow("Basic")
        << (QList<QString>() << ":/assets/me.jpg")
        << QString("Fav");
}

void TestLocalDatabase::playListRemove()
{
    PlayList pl;
    pl.addSinglePath(":/assents/me.jpg");

    PlayListRecordBuilder plrBuilder;
    plrBuilder
        .setName("test_remove")
        .setCoverPath("cover.jpg")
        .setPlayList(&pl)
        .setType(LocalPlayListProviderFactory::TYPE);
    PlayListRecord *record = plrBuilder.obtain();
    record->save();

    QList<PlayListRecord *> recordsAfterSave = PlayListRecord::all();
    QList<PlayListRecord *>::iterator it = recordsAfterSave.begin();
    while (it != recordsAfterSave.end() &&  (*it)->name() != "test_remove") {
        it++;
    }

    if (it == recordsAfterSave.end()) {
        QFAIL("Cannot find saved playlist.");
        return;
    }

    bool ok = (*it)->remove();
    QCOMPARE(ok, true);

    QList<PlayListRecord *> recordsAfterRemove = PlayListRecord::all();
    it = recordsAfterRemove.begin();
    while (it != recordsAfterRemove.end() && (*it)->name() != "test_remove") {
        it++;
    }

    QCOMPARE(it, recordsAfterRemove.end());

    delete record;
}

void TestLocalDatabase::playListUpdate()
{
    PlayList pl;
    pl.addSinglePath(":/assets/me.jpg");

    PlayListRecordBuilder plrBuilder;
    plrBuilder
        .setName("test_update")
        .setCoverPath("cover.jpg")
        .setPlayList(&pl)
        .setType(LocalPlayListProviderFactory::TYPE);
    PlayListRecord *record = plrBuilder.obtain();
    record->save();

    QList<PlayListRecord *> recordsAfterSave = PlayListRecord::all();
    QList<PlayListRecord *>::iterator it = recordsAfterSave.begin();
    while (it != recordsAfterSave.end() &&  (*it)->name() != "test_update") {
        it++;
    }

    if (it == recordsAfterSave.end()) {
        QFAIL("Cannot find saved playlist.");
        return;
    }

    PlayListRecord *savedRecord = *it;
    savedRecord->setCoverPath("new_cover.jpg");
    savedRecord->setName("test_update_new");
    bool ok = savedRecord->save();

    QCOMPARE(ok, true);

    QList<PlayListRecord *> recordsAfterUpdate = PlayListRecord::all();
    it = recordsAfterUpdate.begin();
    while (it != recordsAfterUpdate.end() && (*it)->name() != "test_update_new") {
        it++;
    }

    QCOMPARE((*it)->coverPath(), QString("new_cover.jpg"));

    delete record;
}

void TestLocalDatabase::insertImagesToPlayList()
{
    ImagePtr imageA(new Image(
        ImageSourceManager::instance()->createSingle(":/assets/me.jpg")));
    ImagePtr imageB(new Image(
        ImageSourceManager::instance()->createSingle(":/assets/silica.png")));

    PlayList pl;
    pl.append(imageA);

    PlayListRecordBuilder plrBuilder;
    plrBuilder
        .setName("test_insert_images_to_playlist")
        .setCoverPath("")
        .setPlayList(&pl)
        .setType(LocalPlayListProviderFactory::TYPE);
    PlayListRecord *record = plrBuilder.obtain();
    record->save();
    int rid = record->id();

    QVERIFY2(record->isSaved(), "PlayListRecord should be saved.");
    QCOMPARE(record->playList()->count(), 1);

    ImageList images;
    images << imageB;
    record->insertImages(images);
    delete record;

    PlayListRecordBuilder plrBuilder2;
    plrBuilder2
        .setName("test_insert_images_to_playlist")
        .setCoverPath("")
        .setId(rid)
        .setType(LocalPlayListProviderFactory::TYPE);
    PlayListRecord *record2 = plrBuilder2.obtain();

    PlayList *spl = record2->playList();
    QCOMPARE(spl->count(), 2);

    delete record2;
}

void TestLocalDatabase::removeImagesFromPlayList()
{
    PlayList pl;
    ImagePtr imageA = pl.addSinglePath(":/assets/me.jpg");
    ImagePtr imageB = pl.addSinglePath(":/assets/silica.png");

    PlayListRecordBuilder plrBuilder;
    plrBuilder
        .setName("test_remove_images_from_playlist")
        .setCoverPath("")
        .setPlayList(&pl)
        .setType(LocalPlayListProviderFactory::TYPE);
    PlayListRecord *record = plrBuilder.obtain();
    record->save();
    int rid = record->id();

    QVERIFY2(record->isSaved(), "PlayListRecord should be saved.");
    QCOMPARE(record->playList()->count(), 2);

    QVERIFY(record->removeImage(imageB));
    delete record;

    PlayListRecordBuilder plrBuilder2;
    plrBuilder2
        .setName("test_remove_images_from_playlist")
        .setCoverPath("")
        .setId(rid)
        .setType(LocalPlayListProviderFactory::TYPE);
    PlayListRecord *record2 = plrBuilder2.obtain();

    PlayList *spl = record2->playList();
    QCOMPARE(spl->count(), 1);
    QCOMPARE(spl->at(0)->source()->hashStr(), imageA->source()->hashStr());

    delete record2;
}

void TestLocalDatabase::insertImage()
{
    QFETCH(QString, imagePath);

    Image image(imagePath);

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
