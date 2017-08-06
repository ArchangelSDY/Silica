#include <QSignalSpy>
#include <QTest>

#include "STestCase.h"
#include "../src/GlobalConfig.h"
#include "../src/image/Image.h"
#include "../src/image/ImageSourceManager.h"
#include "../src/ui/ImageGalleryItem.h"
#include "../src/ui/renderers/CompactRendererFactory.h"
#include "../src/ui/renderers/LooseRendererFactory.h"
#include "../src/ui/renderers/WaterfallRendererFactory.h"

class TestImageGalleryItem : public STestCase
{
    Q_OBJECT
private slots:
    void load();
    void load_data();
};

Q_DECLARE_METATYPE(AbstractRendererFactory *)

void TestImageGalleryItem::load()
{
    QFETCH(AbstractRendererFactory *, rendererFactory);
    QFETCH(QString, imagePath);

    ImageSource *src = ImageSourceManager::instance()->createSingle(imagePath);
    ImagePtr image = ImagePtr::create(src);

    ImageGalleryItem item(image, rendererFactory);
    QSignalSpy spy(&item, SIGNAL(readyToShow()));
    QCOMPARE(item.isReadyToShow(), false);
    item.load();
    QVERIFY(spy.wait(60000));
    QCOMPARE(item.isReadyToShow(), true);
}

void TestImageGalleryItem::load_data()
{
    QTest::addColumn<AbstractRendererFactory *>("rendererFactory");
    QTest::addColumn<QString>("imagePath");

    QTest::newRow("Loose")
        << static_cast<AbstractRendererFactory *>(new LooseRendererFactory())
        << ":/assets/silica.png";
    QTest::newRow("Compact")
        << static_cast<AbstractRendererFactory *>(new CompactRendererFactory())
        << ":/assets/silica.png";
    QTest::newRow("Waterfall")
        << static_cast<AbstractRendererFactory *>(new WaterfallRendererFactory())
        << ":/assets/silica.png";
}


QTEST_MAIN(TestImageGalleryItem)
#include "ImageGalleryItem_Tests.moc"
