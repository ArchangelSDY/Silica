#include <QTest>

#include "STestCase.h"
#include "Definitions.h"

class TestDefinitions : public STestCase
{
    Q_OBJECT
private slots:
    void buildEnv();
};

void TestDefinitions::buildEnv()
{
    // Tests should run under test environment
    QCOMPARE(g_BUILD_ENV, "Silica-Test");
}

QTEST_MAIN(TestDefinitions)
#include "Definitions_Tests.moc"

