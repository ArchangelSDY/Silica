#include <QList>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTest>

#include "STestCase.h"
#include "../src/logger/listeners/ImagePathCorrector.h"
#include "../src/logger/LogRecord.h"

class TestImagePathCorrector : public STestCase
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanup();
    void correct();
    void correct_data();

private:
    QTemporaryDir testDir;
};

void TestImagePathCorrector::initTestCase()
{
    STestCase::initTestCase();

    if (!testDir.isValid()) {
        QFAIL("Cannot create test dir.");
    }
}

void TestImagePathCorrector::cleanup()
{
    testDir.remove();

    QDir dir;
    dir.mkpath(testDir.path());
}

class MockPromptClient : public QObject, public ImagePathCorrector::PromptClient
{
    Q_OBJECT
public:
    void prompt(QList<ImagePathCorrector::PathPatch> &aPatches)
    {
        patches = aPatches;
        emit onPrompt();
    }

    QList<ImagePathCorrector::PathPatch> patches;

signals:
    void onPrompt();
};


void TestImagePathCorrector::correct()
{
    QFETCH(QUrl, oldUrl);
    QFETCH(QUrl, newUrl);

    // Ensure new url exists
    QFileInfo newFileInfo(newUrl.path());
    QDir dir;
    dir.mkpath(newFileInfo.dir().absolutePath());
    QFile newFile(newUrl.path());
    if (newFile.open(QIODevice::WriteOnly)) {
        newFile.write("42");
        newFile.close();
    } else {
        QFAIL("Fail to create test file");
    }

    QList<QString> searchDirs;
    searchDirs << QString("%1/new").arg(testDir.path());

    MockPromptClient promptClient;
    ImagePathCorrector corrector(searchDirs, 0, &promptClient);
    corrector.setTestMode(true);

    LogRecord record;
    record.extra.insert("ImageURL", oldUrl);
    record.extra.insert("ImageHashStr", "hash");

    corrector.dispatch(record);

    QSignalSpy spy(&promptClient, SIGNAL(onPrompt()));
    QVERIFY(spy.wait());

    QCOMPARE(promptClient.patches.count(), 1);
    QCOMPARE(promptClient.patches[0].newImageUrl, newUrl);

    newFile.remove();
}

void TestImagePathCorrector::correct_data()
{
    QTest::addColumn<QUrl>("oldUrl");
    QTest::addColumn<QUrl>("newUrl");

    QTest::newRow("Local Image")
        << QUrl(QString("file://%1/old/1.jpg").arg(testDir.path()))
        << QUrl(QString("file://%1/new/1.jpg").arg(testDir.path()));
    QTest::newRow("Zip Image")
        << QUrl(QString("zip://%1/old/a.zip#1.jpg").arg(testDir.path()))
        << QUrl(QString("zip://%1/new/a.zip#1.jpg").arg(testDir.path()));
    QTest::newRow("RAR Image")
        << QUrl(QString("rar://%1/old/a.rar#1.jpg").arg(testDir.path()))
        << QUrl(QString("rar://%1/new/a.rar#1.jpg").arg(testDir.path()));
    QTest::newRow("7z Image")
        << QUrl(QString("sevenz://%1/old/a.7z#1.jpg").arg(testDir.path()))
        << QUrl(QString("sevenz://%1/new/a.7z#1.jpg").arg(testDir.path()));

    QTest::newRow("Local Image Deeper")
        << QUrl(QString("file://%1/old/deep/1.jpg").arg(testDir.path()))
        << QUrl(QString("file://%1/new/deep/1.jpg").arg(testDir.path()));
}


QTEST_MAIN(TestImagePathCorrector)
#include "ImagePathCorrector_Tests.moc"
