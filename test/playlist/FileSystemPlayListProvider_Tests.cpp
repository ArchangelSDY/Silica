#include <QFile>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTest>

#include "STestCase.h"
#include "../src/playlist/FileSystemPlayListEntity.h"
#include "../src/playlist/FileSystemPlayListProvider.h"
#include "../src/playlist/PlayListEntity.h"

class TestFileSystemPlayListProvider : public STestCase
{
    Q_OBJECT
private slots:
    void initTestCase() override;
    void loadEntities();
    void triggerEntity();
    void setRootPath();

private:
    QTemporaryDir m_dir;
};

void TestFileSystemPlayListProvider::initTestCase()
{
    STestCase::initTestCase();

    QImage image(":/assets/silica.png");
    QVERIFY(image.save(m_dir.filePath("silica.png")));

    QFile noImage(m_dir.filePath("no-image.txt"));
    QVERIFY(noImage.open(QIODevice::WriteOnly));
    noImage.write("foobar");

    QDir dir(m_dir.path());
    dir.mkpath("sub");
}

void TestFileSystemPlayListProvider::loadEntities()
{
    FileSystemPlayListProvider provider;

    provider.setRootPath(m_dir.path());
    auto entities = provider.loadEntities();
    QCOMPARE(entities.count(), 2);
    QCOMPARE(entities[0]->name(), "silica.png");
    QCOMPARE(entities[1]->name(), "sub");

    qDeleteAll(entities);
}

void TestFileSystemPlayListProvider::setRootPath()
{
    FileSystemPlayListProvider provider;

    QSignalSpy spyRootPathChanged(&provider, &FileSystemPlayListProvider::rootPathChanged);
    QSignalSpy spyEntitiesChanged(&provider, &FileSystemPlayListProvider::entitiesChanged);

    provider.setRootPath(m_dir.path());

    QCOMPARE(provider.rootPath(), m_dir.path());
    QCOMPARE(spyRootPathChanged.count(), 1);
    QCOMPARE(spyRootPathChanged.first()[0].toString(), m_dir.path());
    QCOMPARE(spyEntitiesChanged.count(), 1);

    // Set same path again
    provider.setRootPath(m_dir.path());
    QCOMPARE(spyRootPathChanged.count(), 1);
    QCOMPARE(spyEntitiesChanged.count(), 1);
}

void TestFileSystemPlayListProvider::triggerEntity()
{
    FileSystemPlayListProvider provider;
    provider.setRootPath(m_dir.path());

    QSignalSpy spyRootPathChanged(&provider, &FileSystemPlayListProvider::rootPathChanged);
    QSignalSpy spyEntitiesChanged(&provider, &FileSystemPlayListProvider::entitiesChanged);

    auto entities = provider.loadEntities();
    QCOMPARE(entities.count(), 2);

    // Trigger image
    {
        auto entity = entities[0];
        auto result = provider.triggerEntity(entity);
        QCOMPARE(result, PlayListEntityTriggerResult::LoadPlayList);
    }

    // Trigger dir
    {
        auto entity = entities[1];
        auto result = provider.triggerEntity(entity);
        QCOMPARE(result, PlayListEntityTriggerResult::None);
        QCOMPARE(spyRootPathChanged.count(), 1);
        QCOMPARE(spyEntitiesChanged.count(), 1);
        QCOMPARE(provider.rootPath(), m_dir.filePath("sub"));
    }

    qDeleteAll(entities);
}

QTEST_MAIN(TestFileSystemPlayListProvider)
#include "FileSystemPlayListProvider_Tests.moc"
