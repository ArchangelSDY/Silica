#include <QCoreApplication>
#include <QTest>
#include <QTextCodec>

#include "LocalDatabase.h"
#include "TestLocalDatabase.h"
#include "TestAsunaDatabase.h"
#include "TestImage.h"
#include "TestImageRank.h"
#include "TestImageSource.h"
#include "TestMultiPageReplyIterator.h"
#include "TestPlayList.h"

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    QTextCodec *codec = QTextCodec::codecForName("utf8");
    QTextCodec::setCodecForLocale(codec);

    if (!LocalDatabase::instance()->migrate()) {
        qCritical("Fail to migrate database! Will exit now.");
        return 1;
    }

    int err = 0;
    {
        TestImage testImage;
        err = qMax(err, QTest::qExec(&testImage, app.arguments()));
    }
    {
        TestImageRank testImageRank;
        err = qMax(err, QTest::qExec(&testImageRank, app.arguments()));
    }
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
    {
        TestAsunaDatabase testAsunaDatabase;
        err = qMax(err, QTest::qExec(&testAsunaDatabase, app.arguments()));
    }
    {
        TestMultiPageReplyIterator testMultiPageReplyIterator;
        err = qMax(err, QTest::qExec(&testMultiPageReplyIterator,
                                     app.arguments()));
    }

    if (err != 0) {
        qWarning("There were errors in some of the tests above.");
    }

    return err;
}
