#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QSignalSpy>
#include <QTest>

#include "../src/GlobalConfig.h"
#include "../src/image/Image.h"
#include "../src/db/AsunaDatabase.h"
#include "utils/MultiSignalSpy.h"
#include "deps/QtMockWebServer/src/MockResponse.h"
#include "deps/QtMockWebServer/src/QtMockWebServer.h"
#include "deps/QtMockWebServer/src/RecordedRequest.h"

class TestAsunaDatabase : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void init();
    void cleanup();
    void queryImagesByTag();
    void queryImagesByTag_data();
    void addImageToAlbum();
    void addImageToAlbum_data();

private:
    AsunaDatabase *m_database;
    QtMockWebServer *m_mockServer;
};

Q_DECLARE_METATYPE(MockResponse)

void TestAsunaDatabase::initTestCase()
{
    GlobalConfig::create();
}

void TestAsunaDatabase::init()
{
    m_mockServer = new QtMockWebServer();
    m_mockServer->play();
    m_database = new AsunaDatabase(m_mockServer->getUrl(""));
}

void TestAsunaDatabase::cleanup()
{
    m_mockServer->shutdown();
    delete m_mockServer;
    delete m_database;
}

void TestAsunaDatabase::queryImagesByTag()
{
    QFETCH(QString, tag);
    QFETCH(QList<MockResponse>, responses);
    QFETCH(int, pagesCount);

    foreach (const MockResponse &response, responses) {
        m_mockServer->enqueue(response);
    }

    MultiPageReplyIterator *iter = m_database->queryImagesByTag(tag);
    connect(iter, SIGNAL(finished()), iter, SLOT(deleteLater()));

    MultiSignalSpy gotPageSpy(iter, SIGNAL(gotPage(QNetworkReply*)));
    QVERIFY2(gotPageSpy.wait(pagesCount), "Did not get enough pages.");

    for (int i = 0; i < pagesCount; ++i) {
        RecordedRequest req = m_mockServer->takeRequest();
        QString expectedPath = QString("/api/images/by-tag/pants/?page=%1")
            .arg(i + 1);
        QCOMPARE(req.path(), expectedPath);
    }
}

void TestAsunaDatabase::queryImagesByTag_data()
{
    QTest::addColumn<QString>("tag");
    QTest::addColumn<QList<MockResponse> >("responses");
    QTest::addColumn<int>("pagesCount");

    QTest::newRow("1 page")
        << "pants"
        << (QList<MockResponse>()
                << MockResponse() << MockResponse().setResponseCode(404))
        << 1;
    QTest::newRow("3 page")
        << "pants"
        << (QList<MockResponse>()
                << MockResponse() << MockResponse() << MockResponse()
                << MockResponse().setResponseCode(404))
        << 3;
}

void TestAsunaDatabase::addImageToAlbum()
{
    QFETCH(ImagePtr, image);
    QFETCH(QString, album);
    QFETCH(bool, valid);
    QFETCH(bool, respOk);

    if (respOk) {
        m_mockServer->enqueue(MockResponse()
                              .clearHeaders()
                              .addHeader("Content-Type", "application/json"));
    } else {
        m_mockServer->enqueue(MockResponse()
                              .clearHeaders()
                              .addHeader("Content-Type", "application/json")
                              .setResponseCode(404));
    }

    QNetworkReply *reply = m_database->addImageToAlbum(image, album);
    QCOMPARE(reply != 0, valid);

    if (reply) {
        QSignalSpy spy(reply, SIGNAL(finished()));
        spy.wait();

        QCOMPARE(spy.count(), 1);
        QCOMPARE(reply->error() == QNetworkReply::NoError, respOk);

        // Validate request
        RecordedRequest req = m_mockServer->takeRequest();
        QJsonDocument reqDoc = QJsonDocument::fromJson(req.body());
        QVERIFY2(!reqDoc.isEmpty(), "Request document is empty");
        QJsonObject reqObj = reqDoc.object();
        QJsonValue reqPk = reqObj.value("pk");
        QVERIFY2(!reqPk.isUndefined(), "Pk field not found");
        QCOMPARE(reqPk.toInt(), image->extraInfo().value("pk").toInt());
    }
}

void TestAsunaDatabase::addImageToAlbum_data()
{
    QTest::addColumn<ImagePtr>("image");
    QTest::addColumn<QString>("album");
    QTest::addColumn<bool>("valid");
    QTest::addColumn<bool>("respOk");

    ImagePtr imgWithPk(new Image(QUrl()));
    imgWithPk->extraInfo().insert("pk", 10);
    QTest::newRow("image with pk")
        << imgWithPk
        << "wallpapers"
        << true
        << true;

    ImagePtr imgWithoutPk(new Image(QUrl()));
    QTest::newRow("image without pk")
        << imgWithoutPk
        << "wallpapers"
        << false
        << true;

    QTest::newRow("resp ok")
        << imgWithPk
        << "wallpapers"
        << true
        << true;

    QTest::newRow("resp not ok")
        << imgWithPk
        << "wallpapers"
        << true
        << false;
}

QTEST_MAIN(TestAsunaDatabase)
 #include "AsunaDatabase_Tests.moc"
