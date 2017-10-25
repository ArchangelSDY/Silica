#include <QTest>

#include "../STestCase.h"
#include "../../src/sapi/Message.h"

class TestMessage : public STestCase
{
    Q_OBJECT
private slots:
    void sendAndListen();
};

void TestMessage::sendAndListen()
{
    int recvId1= 0;
    QByteArray recvPayload1;
    auto listener1 = [&recvId1, &recvPayload1](int id, const QByteArray &payload) {
        recvId1 = id;
        recvPayload1 = payload;
    };

    int recvId2= 0;
    QByteArray recvPayload2;
    auto listener2 = [&recvId2, &recvPayload2](int id, const QByteArray &payload) {
        recvId2 = id;
        recvPayload2 = payload;
    };

    sapi::message::addMessageListener("topic1", listener1);
    sapi::message::addMessageListener("topic1", listener2);
    sapi::message::sendMessage("topic1", "p1");
    sapi::message::sendMessage("topic2", "p2");

    QCOMPARE(recvId1, 0);
    QCOMPARE(recvPayload1, QByteArrayLiteral("p1"));

    QCOMPARE(recvId2, 0);
    QCOMPARE(recvPayload2, QByteArrayLiteral("p1"));
}

QTEST_MAIN(TestMessage)
#include "Message_Tests.moc"
