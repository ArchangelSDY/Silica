#include <QApplication>
#include <QDebug>
#include <QProcess>
#include <QScopedPointer>
#include <QThreadPool>

#ifdef Q_OS_WIN
#include "deps/breakpad/client/windows/handler/exception_handler.h"
#endif

#include "GlobalConfig.h"
#include "LocalDatabase.h"
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    GlobalConfig::create();

    // Set crash handler
#ifdef Q_OS_WIN
    QString crashHandlerPipe = GlobalConfig::instance()->crashHandlerPipe();
    QString crashDumpPath = GlobalConfig::instance()->crashDumpPath();
    QStringList crashHandlerArgs = QStringList()
        << crashHandlerPipe
        << crashDumpPath
        << GlobalConfig::instance()->crashUploadHost()
        << QString::number(a.applicationPid())
        << a.arguments();

    QProcess crashHandlerProc;
    crashHandlerProc.setProgram("crashhandler.exe");
    crashHandlerProc.setArguments(crashHandlerArgs);
    crashHandlerProc.start(QIODevice::ReadOnly);
    crashHandlerProc.waitForReadyRead();
    if (crashHandlerProc.read(1) != QByteArray("0")) {
        qWarning() << "Fail to initiate crash handler";
    }
    crashHandlerProc.closeReadChannel(QProcess::StandardOutput);
    crashHandlerProc.closeReadChannel(QProcess::StandardError);
    crashHandlerProc.closeWriteChannel();

    QScopedPointer<google_breakpad::ExceptionHandler> exHandler(new google_breakpad::ExceptionHandler(
        crashDumpPath.toStdWString(),
        nullptr,
        nullptr,
        nullptr,
        google_breakpad::ExceptionHandler::HANDLER_ALL,
        MiniDumpNormal,
        crashHandlerPipe.toStdWString().data(),
        nullptr
    ));
#endif

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
