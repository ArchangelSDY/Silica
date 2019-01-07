#include <QSignalSpy>
#include <QTest>
#include <gmock/gmock.h>

#include "STestCase.h"
#include "../src/GlobalConfig.h"
#include "../src/db/LocalDatabase.h"
#include "../src/image/caches/ImagesCache.h"
#include "../src/Navigator.h"

class MockLocalDatabase : public LocalDatabase
{
    Q_OBJECT
public:
    MOCK_METHOD0(migrate, bool());

    MOCK_METHOD0(queryPlayListRecords, QList<PlayListRecord *>());
    MOCK_METHOD1(queryImageUrlsForLocalPlayListRecord, QList<QUrl>(int playListId));
    MOCK_METHOD1(insertPlayListRecord, bool(PlayListRecord *playListRecord));
    MOCK_METHOD1(removePlayListRecord, bool(PlayListRecord *playListRecord));
    MOCK_METHOD1(updatePlayListRecord, bool(PlayListRecord *playListRecord));
    MOCK_METHOD2(insertImagesForLocalPlayListProvider, bool(const PlayListRecord &record, const ImageList &images));
    MOCK_METHOD2(removeImagesForLocalPlayListProvider, bool(const PlayListRecord &record, const ImageList &images));

    MOCK_METHOD0(queryImagesCount, int());
    MOCK_METHOD1(insertImage, bool(Image *image));
    MOCK_METHOD1(insertImagesAsync, void(const ImageList &images));
    MOCK_METHOD1(queryImageByHashStr, Image*(const QString &hashStr));
    MOCK_METHOD2(updateImageUrl, bool(const QUrl &oldUrl, const QUrl &newUrl));

    MOCK_METHOD1(queryImageRankValue, int(Image *image));
    MOCK_METHOD2(updateImageRank, bool(Image *image, int rank));

    MOCK_METHOD1(insertPluginPlayListProviderType, int(const QString &name));
    MOCK_METHOD1(queryPluginPlayListProviderType, int(const QString &name));

    MOCK_METHOD2(saveTaskProgressTimeConsumption, bool(const QString &key, qint64 timeConsumption));
    MOCK_METHOD1(queryTaskProgressTimeConsumption, qint64(const QString &key));
};

class TestNavigator : public STestCase
{
    Q_OBJECT
private slots:
    void setPlayList();
};

void TestNavigator::setPlayList()
{
    MockLocalDatabase db;
    EXPECT_CALL(db, insertImagesAsync(::testing::SizeIs(1))).Times(1);

    QSharedPointer<ImagesCache> cache = QSharedPointer<ImagesCache>::create(0);
    Navigator navigator(cache, &db);

    QSignalSpy playListChangeSpy(&navigator, &Navigator::playListChange);

    QSharedPointer<PlayList> pl = QSharedPointer<PlayList>::create(QStringList{ QStringLiteral(":/assets/me.jpg") });
    navigator.setPlayList(pl);

    QCOMPARE(playListChangeSpy.takeFirst().takeFirst().value<QSharedPointer<PlayList>>(), pl);
}

int main(int argc, char *argv[])
{
    ::testing::GTEST_FLAG(throw_on_failure) = true;
    ::testing::InitGoogleMock(&argc, argv);
    
    QCoreApplication app(argc, argv);
    app.setAttribute(Qt::AA_Use96Dpi, true);
    TestNavigator tc;
    QTEST_SET_MAIN_SOURCE_PATH
    return QTest::qExec(&tc, argc, argv);
}

#include "Navigator_Tests.moc"

