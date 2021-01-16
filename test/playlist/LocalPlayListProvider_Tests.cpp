#include <QScopedPointer>
#include <QSignalSpy>
#include <QTest>

#include "STestCase.h"
#include "../src/playlist/LocalPlayListEntity.h"
#include "../src/playlist/LocalPlayListProvider.h"
#include "../src/playlist/PlayListEntity.h"
#include "db/MockLocalDatabase.h"

class TestLocalPlayListProvider : public STestCase
{
    Q_OBJECT
private slots:
    void loadEntities();
    void triggerEntity();
    void createEntity();
    void insertEntity();
    void updateEntity();
    void removeEntity();
};

void TestLocalPlayListProvider::loadEntities()
{
    MockLocalDatabase db;
    QList<PlayListEntityData> data = {
        PlayListEntityData(LocalPlayListProvider::TYPE, 1, "foo", 1, "c:/cover1.png"),
        PlayListEntityData(LocalPlayListProvider::TYPE, 1, "bar", 2, "c:/cover2.png"),
    };
    EXPECT_CALL(db, queryPlayListEntities(LocalPlayListProvider::TYPE))
        .Times(1)
        .WillOnce(::testing::Return(data));

    LocalPlayListProvider provider(&db);
    auto entities = provider.loadEntities();
    QCOMPARE(entities.count(), 2);
    QCOMPARE(entities[0]->count(), 1);
    QCOMPARE(entities[0]->name(), "foo");
    QCOMPARE(entities[0]->provider(), &provider);
    QCOMPARE(entities[1]->count(), 2);
    QCOMPARE(entities[1]->name(), "bar");
    QCOMPARE(entities[1]->provider(), &provider);

    qDeleteAll(entities);
}

void TestLocalPlayListProvider::createEntity()
{
    LocalPlayListProvider provider(nullptr);
    QScopedPointer<PlayListEntity> entity(provider.createEntity("name"));
    QCOMPARE(entity->count(), 0);
    QCOMPARE(entity->name(), "name");
    QCOMPARE(entity->provider(), &provider);
}

void TestLocalPlayListProvider::insertEntity()
{
    MockLocalDatabase db;
    EXPECT_CALL(db, insertPlayListEntity(::testing::AllOf(
            ::testing::Field(&PlayListEntityData::name, "name"),
            ::testing::Field(&PlayListEntityData::type, LocalPlayListProvider::TYPE),
            ::testing::Field(&PlayListEntityData::coverPath, "c:/cover.png"))))
        .Times(1)
        .WillOnce([](auto &e) { e.id = 1; return true; });

    LocalPlayListProvider provider(&db);
    QScopedPointer<LocalPlayListEntity> entity(static_cast<LocalPlayListEntity *>(provider.createEntity("name")));
    entity->setCoverImagePath("c:/cover.png");
    QCOMPARE(entity->id(), 0);

    QSignalSpy spy(&provider, &LocalPlayListProvider::entitiesChanged);
    provider.insertEntity(entity.data());
    QCOMPARE(spy.count(), 1);
    QCOMPARE(entity->id(), 1);
}

void TestLocalPlayListProvider::triggerEntity()
{
    LocalPlayListProvider provider(nullptr);
    QCOMPARE(provider.triggerEntity(nullptr), PlayListEntityTriggerResult::LoadPlayList);
}

void TestLocalPlayListProvider::updateEntity()
{
    MockLocalDatabase db;
    EXPECT_CALL(db, updatePlayListEntity(::testing::AllOf(
            ::testing::Field(&PlayListEntityData::id, 1),
            ::testing::Field(&PlayListEntityData::name, "name1"),
            ::testing::Field(&PlayListEntityData::coverPath, "c:/cover1.png"),
            ::testing::Field(&PlayListEntityData::count, 1))))
        .Times(1);

    LocalPlayListProvider provider(&db);
    QScopedPointer<LocalPlayListEntity> entity(new LocalPlayListEntity(&provider, &db, 1, "name1", 1, "c:/cover1.png"));

    QSignalSpy spy(&provider, &LocalPlayListProvider::entitiesChanged);
    provider.updateEntity(entity.data());
    QCOMPARE(spy.count(), 1);
}

void TestLocalPlayListProvider::removeEntity()
{
    MockLocalDatabase db;
    EXPECT_CALL(db, removePlayListEntity(::testing::Eq(1)))
        .Times(1);

    LocalPlayListProvider provider(&db);
    QScopedPointer<LocalPlayListEntity> entity(new LocalPlayListEntity(&provider, &db, 1, "name", 0, "c:/cover.png"));

    QSignalSpy spy(&provider, &LocalPlayListProvider::entitiesChanged);
    provider.removeEntity(entity.data());
    QCOMPARE(spy.count(), 1);
}

int main(int argc, char *argv[])
{
    ::testing::GTEST_FLAG(throw_on_failure) = true;
    ::testing::InitGoogleMock(&argc, argv);

    QCoreApplication app(argc, argv);
    app.setAttribute(Qt::AA_Use96Dpi, true);
    TestLocalPlayListProvider tc;
    QTEST_SET_MAIN_SOURCE_PATH
    return QTest::qExec(&tc, argc, argv);
}

#include "LocalPlayListProvider_Tests.moc"
