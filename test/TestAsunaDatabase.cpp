#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QSignalSpy>
#include <QTest>

#include "Image.h"
#include "TestAsunaDatabase.h"
#include "deps/QtMockWebServer/src/MockResponse.h"
#include "deps/QtMockWebServer/src/RecordedRequest.h"

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

}

void TestAsunaDatabase::queryImagesByTag_data()
{

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
