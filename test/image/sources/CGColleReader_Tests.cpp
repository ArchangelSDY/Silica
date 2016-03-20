#include <QTest>

#include "STestCase.h"
#include "../../../src/image/sources/CGColleReader.h"

class TestCGColleReader : public STestCase
{
    Q_OBJECT
private slots:
    void imageNames();
    void imageNames_data();
    void read();
    void read_data();

};

void TestCGColleReader::imageNames()
{
    QFETCH(QString, res);
    QFETCH(QStringList, imageNames);

    CGColleReader reader(res);
    QVERIFY(reader.open());

    QStringList actImageNames = reader.imageNames();
    QCOMPARE(actImageNames.count(), imageNames.count());

    for (int i = 0; i < actImageNames.count(); ++i) {
        QCOMPARE(actImageNames[i], imageNames[i]);
    }
}

void TestCGColleReader::imageNames_data()
{
    QTest::addColumn<QString>("res");
    QTest::addColumn<QStringList>("imageNames");

    QTest::newRow("Normal")
        << ":/assets/pack.cgc"
        << (QStringList() << "cg_1_1.png" << "cg_1_2.png");
}

void TestCGColleReader::read()
{
    QFETCH(QString, res);

    CGColleReader reader(res);
    QVERIFY(reader.open());

    QStringList imageNames = reader.imageNames();
    foreach (const QString &name, imageNames) {
        QByteArray data = reader.read(name);
        QVERIFY(!data.isEmpty());
    }
}

void TestCGColleReader::read_data()
{
    QTest::addColumn<QString>("res");

    QTest::newRow("Normal")
        << ":/assets/pack.cgc";
}


QTEST_MAIN(TestCGColleReader)
#include "CGColleReader_Tests.moc"
