#include <QList>
#include <QTest>

#include "STestCase.h"
#include "../src/logger/Logger.h"

class TestLogger : public STestCase
{
    Q_OBJECT
private slots:
    void log();
};

void TestLogger::log()
{
    class MockListener : public Logger::LogListener
    {
    public:
        void dispatch(const LogRecord &record) override
        {
            records << record;
        }

        QList<LogRecord> records;
    };

    MockListener listener;

    Logger::instance()->addListener(0, &listener);

    Logger::instance()->log(0)
        .describe("string", "value")
        .describe("bool", true)
        .save();

    Logger::instance()->log(0)
        .describe("int", 42)
        .save();

    Logger::instance()->log(1).save();


    QCOMPARE(listener.records.count(), 2);
    QCOMPARE(listener.records[0].type, 0);
    QCOMPARE(listener.records[0].extra["string"].toString(), QString("value"));
    QCOMPARE(listener.records[0].extra["bool"].toBool(), true);
    QCOMPARE(listener.records[1].type, 0);
    QCOMPARE(listener.records[1].extra["int"].toInt(), 42);
}

QTEST_MAIN(TestLogger)
#include "Logger_Tests.moc"
