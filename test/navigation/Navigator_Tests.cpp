#include <QSignalSpy>
#include <QTest>
#include <QThreadPool>

#include "STestCase.h"
#include "../src/GlobalConfig.h"
#include "../src/db/LocalDatabase.h"
#include "../src/image/caches/ImagesCache.h"
#include "../src/Navigator.h"
#include "db/MockLocalDatabase.h"

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

