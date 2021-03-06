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

    QSharedPointer<Image> image = QSharedPointer<Image>::create(imagePath);
    image->metadata().insert("SPECIAL", 1);
    sapi::ImageDelegate delegate(image);

    QSignalSpy loadSpy(image.data(), &Image::loaded);
    image->load();
    QVERIFY(loadSpy.wait());

    QCOMPARE(delegate.name(), image->name());
    QCOMPARE(delegate.size(), image->size());
    QCOMPARE(delegate.hash(), image->source()->hashStr());
    QCOMPARE(delegate.extraInfo(), image->metadata());
    QCOMPARE(delegate.isAnimation(), image->data().toStrongRef()->isAnimation());
    QCOMPARE(delegate.frameCount(), image->data().toStrongRef()->frames.count());
    QCOMPARE(delegate.durations(), image->data().toStrongRef()->durations);
    for (int i = 0; i < delegate.frameCount(); ++i) {
        QCOMPARE(*(delegate.frames()[i]), image->data().toStrongRef()->frames[i]);
    }
    QVERIFY(!delegate.readRaw().isNull());

    QSignalSpy thumbnailSpy(image.data(), &Image::thumbnailLoaded);
    image->loadThumbnail();
    QVERIFY(thumbnailSpy.wait());
    QVERIFY(!delegate.thumbnail().isNull());
}


void TestImageDelegate::fields_data()
{
    QTest::addColumn<QString>("imagePath");

    QTest::newRow("Normal") << ":/assets/silica.png";
}


QTEST_MAIN(TestImageDelegate)
#include "ImageDelegate_Tests.moc"
