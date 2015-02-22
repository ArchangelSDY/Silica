#include <QBuffer>
#include <QTest>

#include "../STestCase.h"
#include "../../src/sapi/Logging.h"
#include "../../src/sapi/PluginLoggingDelegate.h"

class TestLogging : public STestCase
{
    Q_OBJECT
private slots:
    void debug();
};

void TestLogging::debug()
{
    QString text = "Hello!";
    sapi::debug() << text;
    QCOMPARE(QString(sapi::PluginLoggingDelegate::instance()->content().data()),
             QString("\"%1\" ").arg(text));
}

QTEST_MAIN(TestLogging)
#include "Logging_Tests.moc"
