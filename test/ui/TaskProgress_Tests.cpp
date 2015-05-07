#include <QtSql>
#include <QSignalSpy>
#include <QTest>

#include "STestCase.h"
#include "../src/GlobalConfig.h"
#include "../src/ui/TaskProgress.h"

class TestTaskProgress : public STestCase
{
    Q_OBJECT
private slots:
    void cleanup();
    void constructor();
    void startAndStop();
    void settersAndReset();
    void estimate();
};

void TestTaskProgress::cleanup()
{
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "TestTaskProgress");
        db.setDatabaseName(GlobalConfig::instance()->localDatabasePath());
        db.open();
        db.exec("delete from task_progresses");
    }
    QSqlDatabase::removeDatabase("TestTaskProgress");
}

void TestTaskProgress::constructor()
{
    TaskProgress progress;
    QCOMPARE(progress.value(), 0);
    QCOMPARE(progress.maximum(), 0);
    QCOMPARE(progress.minimum(), 0);
    QCOMPARE(progress.isRunning(), false);
    QVERIFY(progress.key().isEmpty());
}

void TestTaskProgress::startAndStop()
{
    TaskProgress progress;
    QSignalSpy spy(&progress, SIGNAL(changed()));

    progress.start();
    QCOMPARE(spy.count(), 1);
    QCOMPARE(progress.isRunning(), true);

    progress.stop();
    QCOMPARE(spy.count(), 2);
    QCOMPARE(progress.isRunning(), false);
}

void TestTaskProgress::settersAndReset()
{
    TaskProgress progress;
    QSignalSpy spy(&progress, SIGNAL(changed()));
    int sigCnt = 0;

    progress.start();
    QCOMPARE(spy.count(), ++sigCnt);

    progress.setKey("Test");
    QCOMPARE(progress.key(), QString("Test"));
    QCOMPARE(spy.count(), ++sigCnt);

    progress.setMaximum(10);
    QCOMPARE(progress.maximum(), 10);
    QCOMPARE(spy.count(), ++sigCnt);

    progress.setMinimum(2);
    QCOMPARE(progress.minimum(), 2);
    QCOMPARE(spy.count(), ++sigCnt);

    progress.setValue(8);
    QCOMPARE(progress.value(), 8);
    QCOMPARE(spy.count(), ++sigCnt);

    progress.setValue(15);
    QCOMPARE(progress.value(), 8);
    QCOMPARE(spy.count(), sigCnt);

    progress.reset();
    QCOMPARE(progress.value(), 0);
    QCOMPARE(progress.minimum(), 0);
    QCOMPARE(progress.maximum(), 0);
    QCOMPARE(progress.isRunning(), false);
    QCOMPARE(spy.count(), ++sigCnt);
}

void TestTaskProgress::estimate()
{
    int timeConsumption = 110;
    int interval = 30;

    TaskProgress progress;
    progress.setKey("TEST_TASK_PROGRESS");
    progress.setEstimateEnabled(true);
    progress.setEstimateInterval(interval);
    progress.setMaximum(10);

    QSignalSpy spy(&progress, SIGNAL(changed()));

    // For the first time it makes no estimation and only records
    // the total time consumption
    progress.start();
    QTest::qWait(timeConsumption);
    progress.stop();
    QCOMPARE(spy.count(), 2);

    progress.reset();
    progress.setMaximum(10);
    spy.clear();

    // For the second time and later we will receive estimated progress
    // update
    progress.start();
    QTest::qWait(timeConsumption);
    progress.stop();
    QCOMPARE(spy.count(), 2 + timeConsumption / interval);
}

QTEST_MAIN(TestTaskProgress)
#include "TaskProgress_Tests.moc"
