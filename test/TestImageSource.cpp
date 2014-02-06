#include <QTest>
#include <QUrl>

#include "../src/ImageSource.h"

#include "TestImageSource.h"

void TestImageSource::open()
{
    QFETCH(QUrl, srcUrl);

    ImageSource *imageSource = ImageSource::create(srcUrl);
    bool result = imageSource->open();
    QCOMPARE(result, true);
    QVERIFY(imageSource->device() != 0);
    QCOMPARE(imageSource->device()->isOpen(), true);
    QCOMPARE(imageSource->device()->isReadable(), true);
    delete imageSource;
}

void TestImageSource::open_data()
{
    QTest::addColumn<QUrl>("srcUrl");

    const QString &currentDir = qApp->applicationDirPath();
    QTest::newRow("Zip Source")
        << QUrl("zip:///" + currentDir + "/assets/pack.zip#silicå.png");
    QTest::newRow("Image Source")
        << QUrl("file:///" + currentDir + "/assets/me.jpg");
}
