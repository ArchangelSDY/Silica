#include <QTest>

#include "TestZipImageSource.h"

void TestZipImageSource::init()
{
    m_zipImageSource = new ZipImageSource(
        QCoreApplication::applicationDirPath() + "/assets/pack.zip",
        "silicå.png");
}

void TestZipImageSource::cleanup()
{
    delete m_zipImageSource;
}

void TestZipImageSource::open()
{
    bool result = m_zipImageSource->open();
    QCOMPARE(result, true);
    QVERIFY(m_zipImageSource->device() != 0);
    QCOMPARE(m_zipImageSource->device()->isOpen(), true);
    QCOMPARE(m_zipImageSource->device()->isReadable(), true);
}
