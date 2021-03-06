#include <QTemporaryDir>
#include <QTest>
#include <QUrl>

#include "STestCase.h"
#include "../src/image/ImageSource.h"
#include "../src/image/ImageSourceManager.h"

class TestImageSource : public STestCase
{
    Q_OBJECT
private slots:
    void openAndClose();
    void openAndClose_data();
    void properties();
    void properties_data();
    void copy();
    void copy_data();
};

void TestImageSource::openAndClose()
{
    QFETCH(QUrl, srcUrl);

    ImageSource *imageSource =
        ImageSourceManager::instance()->createSingle(srcUrl);
    QVERIFY(imageSource->open());
    QVERIFY(imageSource->device() != 0);
    QVERIFY(imageSource->device()->isOpen());
    QVERIFY(imageSource->device()->isReadable());

    imageSource->close();
    QVERIFY(imageSource->device() == 0);

    delete imageSource;
}

void TestImageSource::openAndClose_data()
{
    QTest::addColumn<QUrl>("srcUrl");

    const QString &currentDir = qApp->applicationDirPath();
    QTest::newRow("Zip Source: Latin Name")
        << QUrl("zip:/" + currentDir + "/assets/pack.zip#me.jpg");
    QTest::newRow("Zip Source: UTF-8 Name")
        << QUrl("zip:/" + currentDir + "/assets/pack.zip#silicå.png");
    QTest::newRow("Local Source")
        << QUrl("file:///" + currentDir + "/assets/me.jpg");
}

void TestImageSource::properties()
{
    QFETCH(QUrl, srcUrl);
    QFETCH(QString, name);

    ImageSource *imageSource =
        ImageSourceManager::instance()->createSingle(srcUrl);
    imageSource->open();
    QCOMPARE(imageSource->name(), name);
    QVERIFY(!imageSource->hash().isEmpty());
    QVERIFY(!imageSource->hash().isNull());

    delete imageSource;
}

void TestImageSource::properties_data()
{
    QTest::addColumn<QUrl>("srcUrl");
    QTest::addColumn<QString>("name");

    const QString &currentDir = qApp->applicationDirPath();
    QTest::newRow("Zip Source")
        << QUrl("zip:/" + currentDir + "/assets/pack.zip#silicå.png")
        << "silicå.png";
    QTest::newRow("Local Source")
        << QUrl("file:///" + currentDir + "/assets/me.jpg")
        << "me.jpg";
}

void TestImageSource::copy()
{
    QFETCH(QUrl, srcUrl);
    QTemporaryDir tmpDir;

    ImageSource *imageSource =
        ImageSourceManager::instance()->createSingle(srcUrl);
    imageSource->open();

    QString destPath = tmpDir.path() + "/" + imageSource->name();
    QVERIFY(imageSource->copy(destPath));
    QFileInfo destInfo(destPath);
    QVERIFY(destInfo.exists());

    delete imageSource;
}

void TestImageSource::copy_data()
{
    QTest::addColumn<QUrl>("srcUrl");

    const QString &currentDir = qApp->applicationDirPath();
    QTest::newRow("Zip Source")
        << QUrl("zip:/" + currentDir + "/assets/pack.zip#silicå.png");
    QTest::newRow("Local Source")
        << QUrl("file:///" + currentDir + "/assets/me.jpg");
}

QTEST_MAIN(TestImageSource)
 #include "ImageSource_Tests.moc"
