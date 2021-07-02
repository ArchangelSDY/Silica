#include <QScopedPointer>
#include <QSignalSpy>
#include <QTest>
#include <gmock/gmock.h>

#include "STestCase.h"
#include "../src/playlist/LocalPlayListEntity.h"
#include "../src/playlist/LocalPlayListProvider.h"
#include "../src/GlobalConfig.h"
#include "db/MockLocalDatabase.h"

class MockLocalPlayListProvider : public LocalPlayListProvider
{
public:
    MockLocalPlayListProvider(LocalDatabase *db)
        : LocalPlayListProvider(db)
    {
    }

    MOCK_METHOD1(updateEntity, void(PlayListEntity *));
};

class TestLocalPlayListEntity : public STestCase
{
    Q_OBJECT
public:
    TestLocalPlayListEntity();

private slots:
    void init();
    void cleanup();
    void properties();
    void addImageUrls();
    void removeImageUrls();
    void loadCoverImage();
    void loadImageUrls();

private:
    QString m_coverPath;
};

TestLocalPlayListEntity::TestLocalPlayListEntity()
{
    m_coverPath = "TestLocalPlayListEntity_coverImage.jpg";
}

void TestLocalPlayListEntity::init()
{
    QImage coverImage(":/assets/silica.png");
    QVERIFY(!coverImage.isNull());
    QVERIFY(coverImage.save(GlobalConfig::instance()->thumbnailPath() + "/" + m_coverPath));
}

void TestLocalPlayListEntity::cleanup()
{
    QFile::remove(GlobalConfig::instance()->thumbnailPath() + "/" + m_coverPath);
}

void TestLocalPlayListEntity::properties()
{
    LocalPlayListEntity entity(nullptr, nullptr, 1, "name", 0, "c:/cover.png");

    QCOMPARE(entity.id(), 1);

    QCOMPARE(entity.name(), "name");
    entity.setName("name1");
    QCOMPARE(entity.name(), "name1");

    QCOMPARE(entity.count(), 0);

    QCOMPARE(entity.coverImagePath(), "c:/cover.png");
    entity.setCoverImagePath("c:/cover1.png");
    QCOMPARE(entity.coverImagePath(), "c:/cover1.png");
}

void TestLocalPlayListEntity::addImageUrls()
{
    MockLocalDatabase db;
    MockLocalPlayListProvider provider(&db);

    LocalPlayListEntity entity(&provider, &db, 1, "name", 0, "c:/cover.png");
    QCOMPARE(entity.count(), 0);

    QList<QUrl> imageUrls { QUrl::fromLocalFile("c:/image.png") };

    EXPECT_CALL(db, insertLocalPlayListEntityImageUrls(
            ::testing::Eq(1),
            ::testing::Ref(imageUrls)))
        .Times(1);
    EXPECT_CALL(provider, updateEntity(::testing::Eq(&entity)))
        .Times(1);

    entity.addImageUrls(imageUrls);
    QCOMPARE(entity.count(), 1);
}

void TestLocalPlayListEntity::removeImageUrls()
{
    MockLocalDatabase db;
    MockLocalPlayListProvider provider(&db);

    LocalPlayListEntity entity(&provider, &db, 1, "name", 1, "c:/cover.png");
    QCOMPARE(entity.count(), 1);

    QList<QUrl> imageUrls { QUrl::fromLocalFile("c:/image.png") };

    EXPECT_CALL(db, removeLocalPlayListEntityImageUrls(
            ::testing::Eq(1),
            ::testing::Ref(imageUrls)))
        .Times(1);
    EXPECT_CALL(provider, updateEntity(::testing::Eq(&entity)))
        .Times(1);

    entity.removeImageUrls(imageUrls);
    QCOMPARE(entity.count(), 0);
}

void TestLocalPlayListEntity::loadCoverImage()
{
    LocalPlayListEntity entity(nullptr, nullptr, 1, "name", 0, m_coverPath);
    auto coverImage = entity.loadCoverImage();
    QVERIFY(!coverImage.isNull());
}

void TestLocalPlayListEntity::loadImageUrls()
{
    MockLocalDatabase db;

    QList<QUrl> imageUrls{ QUrl::fromLocalFile("c:/image.png") };
    EXPECT_CALL(db, queryLocalPlayListEntityImageUrls(1))
        .Times(1)
        .WillOnce(::testing::Return(imageUrls));

    LocalPlayListEntity entity(nullptr, &db, 1, "name", 1, m_coverPath);
    auto loadedImageUrls = entity.loadImageUrls(nullptr);
    QCOMPARE(loadedImageUrls.count(), 1);
    QCOMPARE(loadedImageUrls[0].toLocalFile(), "c:/image.png");
}

int main(int argc, char *argv[])
{
    ::testing::GTEST_FLAG(throw_on_failure) = true;
    ::testing::InitGoogleMock(&argc, argv);

    QCoreApplication app(argc, argv);
    app.setAttribute(Qt::AA_Use96Dpi, true);
    TestLocalPlayListEntity tc;
    QTEST_SET_MAIN_SOURCE_PATH
    return QTest::qExec(&tc, argc, argv);
}

#include "LocalPlayListEntity_Tests.moc"
