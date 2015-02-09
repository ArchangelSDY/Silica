#include "STestCase.h"

#include <QDir>
#include <QTest>

#include "../src/GlobalConfig.h"
#include "../src/db/LocalDatabase.h"

void STestCase::initTestCase()
{
    Q_INIT_RESOURCE(silica);
    Q_INIT_RESOURCE(silicatest);
    GlobalConfig::create();
    QVERIFY2(LocalDatabase::instance()->migrate(),
             "Fail to migrate database");
    QDir thumbnailDir(GlobalConfig::instance()->thumbnailPath());
    thumbnailDir.removeRecursively();
}

