#include <QSignalSpy>
#include <QTest>
#include <QThreadPool>
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

    MOCK_METHOD1(queryPlayListEntities, QList<PlayListEntityData>(int type));
    MOCK_METHOD1(queryPlayListEntity, PlayListEntityData(int playListId));
    MOCK_METHOD1(insertPlayListEntity, bool(PlayListEntityData &data));
    MOCK_METHOD1(removePlayListEntity, bool(int playListId));
    MOCK_METHOD1(updatePlayListEntity, bool(const PlayListEntityData &data));

    MOCK_METHOD1(queryLocalPlayListEntityImageUrls, QList<QUrl>(int playListId));
    MOCK_METHOD2(insertLocalPlayListEntityImageUrls, bool(int playListId, const QList<QUrl> &images));
    MOCK_METHOD2(removeLocalPlayListEntityImageUrls, bool(int playListId, const QList<QUrl> &images));

    MOCK_METHOD0(queryImagesCount, int());
    MOCK_METHOD1(insertImage, bool(Image *image));
    MOCK_METHOD1(insertImages, bool(const ImageList &images));
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
    EXPECT_CALL(db, insertImage(::testing::Property(&Image::size, ::testing::Eq(QSize(109, 109))))).Times(1);

    QSharedPointer<ImagesCache> cache = QSharedPointer<ImagesCache>::create(0);
    Navigator navigator(cache, &db);

    QSignalSpy playListChangeSpy(&navigator, &Navigator::playListChange);
    QSignalSpy thumbnailSpy(&navigator, &Navigator::paintThumbnail);

    QSharedPointer<PlayList> pl = QSharedPointer<PlayList>::create(QStringList{ QStringLiteral(":/assets/me.jpg") });
    navigator.setPlayList(pl);

    QCOMPARE(playListChangeSpy.takeFirst().takeFirst().value<QSharedPointer<PlayList>>(), pl);
    QVERIFY(thumbnailSpy.wait());
    QCOMPARE(thumbnailSpy.count(), 1);
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

