#include <QTest>

#include "STestCase.h"
#include "../../../src/image/sources/CGColleV0Reader.h"

class TestCGColleV0Reader : public STestCase
{
    Q_OBJECT
private slots:
    void imageNames();
    void imageNames_data();
    void read();
    void read_data();

};

void TestCGColleV0Reader::imageNames()
{
    QFETCH(QString, res);
    QFETCH(QStringList, imageNames);

    CGColleV0Reader reader(res);
    QVERIFY(reader.open());

    QStringList actImageNames = reader.imageNames();
    QCOMPARE(actImageNames.count(), imageNames.count());

    for (int i = 0; i < actImageNames.count(); ++i) {
        QCOMPARE(actImageNames[i], imageNames[i]);
    }
}

void TestCGColleV0Reader::imageNames_data()
{
    QTest::addColumn<QString>("res");
    QTest::addColumn<QStringList>("imageNames");

    QTest::newRow("Normal")
        << ":/assets/pack-v0.cgc"
        << (QStringList() << "cg_1_1.png" << "cg_1_2.png");
}

void TestCGColleV0Reader::read()
{
    QFETCH(QString, res);

    CGColleV0Reader reader(res);
    QVERIFY(reader.open());

    QStringList imageNames = reader.imageNames();
    foreach (const QString &name, imageNames) {
        QScopedPointer<CGColleReader::ImageReader> imageReader(reader.createReader(name));
        QVERIFY(!imageReader.isNull());
        QVERIFY(!imageReader->read().isEmpty());
    }
}

void TestCGColleV0Reader::read_data()
{
    QTest::addColumn<QString>("res");

    QTest::newRow("Normal")
        << ":/assets/pack-v0.cgc";
}


QTEST_MAIN(TestCGColleV0Reader)
#include "CGColleV0Reader_Tests.moc"
