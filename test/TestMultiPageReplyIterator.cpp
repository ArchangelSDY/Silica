#include <QNetworkRequest>
#include <QSignalSpy>
#include <QTest>

#include "../src/db/MultiPageReplyIterator.h"
#include "deps/QtMockWebServer/src/MockResponse.h"
#include "deps/QtMockWebServer/src/QtMockWebServer.h"
#include "deps/QtMockWebServer/src/QueueDispatcher.h"
#include "deps/QtMockWebServer/src/RecordedRequest.h"

#include "TestMultiPageReplyIterator.h"
#include "utils/MultiSignalSpy.h"

Q_DECLARE_METATYPE(MockResponse)

void TestMultiPageReplyIterator::init()
{
    m_mockServer = new QtMockWebServer();
    m_mockServer->play();
}

void TestMultiPageReplyIterator::cleanup()
{
    m_mockServer->shutdown();
    delete m_mockServer;
}

void TestMultiPageReplyIterator::gotPage()
{
    QFETCH(int, pageStart);
    QFETCH(QList<MockResponse>, responses);
    QFETCH(int, pagesCount);

    QueueDispatcher *failFastDispatcher = new QueueDispatcher();
    failFastDispatcher->setFailFast(true);
    m_mockServer->setDispatcher(failFastDispatcher);

    foreach(const MockResponse &response, responses) {
        m_mockServer->enqueue(response);
    }

    QNetworkRequest req(
        m_mockServer->getUrl(QString("/?page=%1").arg(pageStart)));
    QNetworkReply *reply = m_mgr.get(req);
    MultiPageReplyIterator iter(reply);
    QSignalSpy finishedSpy(&iter, SIGNAL(finished()));

    MultiSignalSpy gotPageSpy(&iter, SIGNAL(gotPage(QNetworkReply *)));
    QVERIFY2(gotPageSpy.wait(pagesCount), "Did not get enough pages.");

    for (int i = 0; i < pagesCount; ++i) {
        RecordedRequest req = m_mockServer->takeRequest();
        bool correctPageQueryExists =
            req.path().contains(QString("page=%1").arg(i + pageStart));
        QVERIFY2(correctPageQueryExists, "Wrong request path");
    }

    if (finishedSpy.count() < 1) {
        finishedSpy.wait();
    }
    QCOMPARE(finishedSpy.count(), 1);
}

void TestMultiPageReplyIterator::gotPage_data()
{
    QTest::addColumn<int>("pageStart");
    QTest::addColumn<QList<MockResponse> >("responses");
    QTest::addColumn<int>("pagesCount");

    QTest::newRow("1 page")
        << 1
        << (QList<MockResponse>()
                << MockResponse())
        << 1;
    QTest::newRow("3 page")
        << 1
        << (QList<MockResponse>()
                << MockResponse() << MockResponse() << MockResponse())
        << 3;
    QTest::newRow("3 page with next not found")
        << 1
        << (QList<MockResponse>()
                << MockResponse() << MockResponse() << MockResponse()
                << MockResponse().setStatus("404"))
        << 3;
}
