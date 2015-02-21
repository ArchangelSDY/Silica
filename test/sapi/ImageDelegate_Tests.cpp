#include <QSignalSpy>
#include <QTest>

#include "STestCase.h"
#include "../../src/image/Image.h"
#include "../../src/image/ImageSource.h"
#include "../../src/sapi/ImageDelegate.h"

class TestImageDelegate : public STestCase
{
    Q_OBJECT
private slots:
    void fields();
    void fields_data();
};

void TestImageDelegate::fields()
{
    QFETCH(QString, imagePath);

    Image image(imagePath);
    image.extraInfo().insert("SPECIAL", 1);
    sapi::ImageDelegate delegate(&image);

    QSignalSpy loadSpy(&image, SIGNAL(loaded()));
    QSignalSpy makeThumbnailSpy(&image, SIGNAL(thumbnailLoaded()));
    image.load();
    QVERIFY(loadSpy.wait());

    QCOMPARE(delegate.name(), image.name());
    QCOMPARE(delegate.size(), image.size());
    QCOMPARE(delegate.hash(), image.source()->hashStr());
    QCOMPARE(delegate.extraInfo(), image.extraInfo());
    QCOMPARE(delegate.isAnimation(), image.isAnimation());
    QCOMPARE(delegate.frameCount(), image.frameCount());
    QCOMPARE(delegate.durations(), image.durations());
    for (int i = 0; i < delegate.frameCount(); ++i) {
        QCOMPARE(*(delegate.frames()[i]), *(image.frames()[i]));
    }

    if (makeThumbnailSpy.count() == 0) {
        QVERIFY(makeThumbnailSpy.wait());
    }
    QCOMPARE(delegate.thumbnail(), image.thumbnail());
    QVERIFY(!delegate.readRaw().isNull());
}


void TestImageDelegate::fields_data()
{
    QTest::addColumn<QString>("imagePath");

    QTest::newRow("Normal") << ":/assets/silica.png";
}


QTEST_MAIN(TestImageDelegate)
#include "ImageDelegate_Tests.moc"
