#include <QApplication>
#include <QtGlobal>

#include "LocalDatabase.h"
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("Asuna");
    QCoreApplication::setApplicationName("Silica");

    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope,
                       QCoreApplication::applicationDirPath() + "/config");

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
