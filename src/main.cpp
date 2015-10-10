#include <QApplication>
#include <QtGlobal>

#include "GlobalConfig.h"
#include "LocalDatabase.h"
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    GlobalConfig::create();

    if (!LocalDatabase::instance()->migrate()) {
        qCritical("Fail to migrate database! Will exit now.");
        return 1;
    }

    QThreadPool::globalInstance()->setMaxThreadCount(
        QThread::idealThreadCount() - 1);

    MainWindow w;
    w.show();

    const int ret = a.exec();

    QThreadPool::globalInstance()->clear();
    return ret;
}
