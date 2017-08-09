#include <QTest>

#include "../STestCase.h"

#include "../../src/keychain/KeyChain.h"

#ifdef Q_OS_WIN
#include <windows.h>
#include <wincred.h>
#endif

class TestKeyChain : public STestCase
{
    Q_OBJECT
private slots:
    void writeRead();
    void cleanup();

private:
    static QString KEY;
};

QString TestKeyChain::KEY = "silica-test-key-chain";

#ifdef Q_OS_WIN
void TestKeyChain::writeRead()
{
    KeyChain keyChain;
    QByteArray data("foobar");

    QVERIFY(keyChain.write(KEY, data, ""));
    QCOMPARE(keyChain.read(KEY), data);
}

void TestKeyChain::cleanup()
{
    CredDeleteW((LPCWSTR)KEY.utf16(), CRED_TYPE_GENERIC, 0);
}
#else
void TestKeyChain::writeRead()
{
}

void TestKeyChain::cleanup()
{
}
#endif

QTEST_MAIN(TestKeyChain)
#include "KeyChain_Tests.moc"
