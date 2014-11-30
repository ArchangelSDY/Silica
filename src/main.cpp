#include <QApplication>
#include <QtGlobal>

#include "LocalDatabase.h"
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // TODO: move to a better place
    qRegisterMetaType<QList<QImage *> >("QList<QImage*>");
    qRegisterMetaType<QList<int> >("QList<int>");

    QCoreApplication::setOrganizationName("Asuna");
    QCoreApplication::setApplicationName("Silica");

    if (!LocalDatabase::instance()->migrate()) {
        qCritical("Fail to migrate database! Will exit now.");
        return 1;
    }

    QTextCodec *codec = QTextCodec::codecForName("utf8");
    QTextCodec::setCodecForLocale(codec);

    QThreadPool::globalInstance()->setMaxThreadCount(
        QThread::idealThreadCount() - 1);

    MainWindow w;
    w.show();

    const int ret = a.exec();

    QThreadPool::globalInstance()->clear();
    return ret;
}
