#include <QtSql>
#include <QSignalSpy>
#include <QTest>

#include "../STestCase.h"
#include "../../src/playlist/PlayListProvider.h"
#include "../../src/playlist/PlayListProviderFactory.h"
#include "../../src/playlist/PlayListProviderManager.h"
#include "../../src/playlist/PlayListRecord.h"
#include "../../src/GlobalConfig.h"
#include "../../src/PlayList.h"

class MockProvider : public PlayListProvider
{
public:
    // PlayListProvider interfaces
    QString typeName() const override { return "Mock"; }
    bool canContinueProvide() const override { return false; }

    void request(const QString &name, const QVariantHash &extra) override
    {
        QList<QUrl> images;
        QList<QVariantHash> extraInfos;

        for (int i = 0; i < itemsCount; ++i) {
            images << QUrl("file:///tmp/test.png");
            extraInfos << QVariantHash();
        }

        emit gotItems(images, extraInfos);
    }

    // For tests
    MockProvider() : itemsCount(0) {}
    int itemsCount;
};

class MockProviderFactory : public PlayListProviderFactory
{
public:
    PlayListProvider *create() override { return &provider; }
    MockProvider provider;
    static const int TYPE = -100;
};

class TestPlayListRecord: public STestCase
{
    Q_OBJECT
private slots:
    void initTestCase() override;

    void onProviderItemsCountChanged();
};

void TestPlayListRecord::initTestCase()
{
    STestCase::initTestCase();

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "TestLocalDatabase");
    db.setDatabaseName(GlobalConfig::instance()->localDatabasePath());
    db.open();
    db.exec("delete from playlists");
    db.close();
    QSqlDatabase::removeDatabase("TestPlayListRecord");
}

void TestPlayListRecord::onProviderItemsCountChanged()
{
    MockProviderFactory providerFactory;
    providerFactory.provider.itemsCount = 1;
    PlayListProviderManager::instance()->registerProvider(
        MockProviderFactory::TYPE, &providerFactory);

    QString name = "test-items-count-changed";
    PlayListRecordBuilder plrBuilder;
    plrBuilder
        .setName(name)
        .setCoverPath("/cover/path.png")
        .setType(MockProviderFactory::TYPE);
    PlayListRecord *recordToSave = plrBuilder.obtain();

    QVERIFY(recordToSave->save());

    // Check update count from provider
    QSharedPointer<PlayList> pl = recordToSave->playList();
    QCOMPARE(pl->count(), providerFactory.provider.itemsCount);
    QCOMPARE(recordToSave->count(), providerFactory.provider.itemsCount);

    // Check database
    QList<PlayListRecord *> records = PlayListRecord::all();
    PlayListRecord *savedRecord = 0;
    foreach (PlayListRecord *record, records) {
        if (record->id() == recordToSave->id()) {
            savedRecord = record;
            break;
        }
    }
    QVERIFY(savedRecord != 0);
    QCOMPARE(savedRecord->count(), providerFactory.provider.itemsCount);

    pl->deleteLater();
    qDeleteAll(records.begin(), records.end());
}

QTEST_MAIN(TestPlayListRecord)
#include "PlayListRecord_Tests.moc"
