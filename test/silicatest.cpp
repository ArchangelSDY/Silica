#include <QCoreApplication>
#include <QTest>
#include <QTextCodec>

#include "TestLocalDatabase.h"
#include "TestImageSource.h"
#include "TestPlayList.h"

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    QTextCodec *codec = QTextCodec::codecForName("utf8");
    QTextCodec::setCodecForLocale(codec);

    int err = 0;
    {
        TestImageSource testImageSource;
        err = qMax(err, QTest::qExec(&testImageSource, app.arguments()));
    }
    {
        TestPlayList testPlayList;
        err = qMax(err, QTest::qExec(&testPlayList, app.arguments()));
    }
    {
        TestLocalDatabase testLocalDatabase;
        err = qMax(err, QTest::qExec(&testLocalDatabase, app.arguments()));
    }

    if (err != 0) {
        qWarning("There were errors in some of the tests above.");
    }

    return err;
}
