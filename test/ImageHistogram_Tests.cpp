#include <QImage>
#include <QTest>

#include "../src/image/ImageHistogram.h"

class TestImageHistogram : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void compare();
    void compare_data();
};

void TestImageHistogram::initTestCase()
{
    Q_INIT_RESOURCE(silicatest);
}

void TestImageHistogram::compare()
{
    QFETCH(QString, imgLeftPath);
    QFETCH(QString, imgRightPath);
    QFETCH(bool, isSimilar);

    QImage imgLeft(imgLeftPath);
    ImageHistogram histLeft(imgLeft);
    QImage imgRight(imgRightPath);
    ImageHistogram histRight(imgRight);

    double result = histLeft.compare(histRight);
    if (isSimilar) {
        QVERIFY(result >= 0.9);
    } else {
        QVERIFY(result < 0.5);
    }
}

void TestImageHistogram::compare_data()
{
    QTest::addColumn<QString>("imgLeftPath");
    QTest::addColumn<QString>("imgRightPath");
    QTest::addColumn<bool>("isSimilar");

    QTest::newRow("Similar")
        << ":/assets/me.jpg" << ":/assets/me.jpg" << true;
    QTest::newRow("Not similar")
        << ":/assets/me.jpg" << ":/assets/silica.png" << false;
    QTest::newRow("Null image")
        << ":/assets/not-found.jpg" << ":assets/me.jpg" << false;
}


QTEST_MAIN(TestImageHistogram)
#include "ImageHistogram_Tests.moc"
