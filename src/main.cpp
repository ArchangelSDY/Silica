#include <QApplication>
#include <QScopedPointer>
#include <QtGlobal>

#ifdef Q_OS_WIN
#include "deps/breakpad/client/windows/handler/exception_handler.h"
#endif

#include "GlobalConfig.h"
#include "LocalDatabase.h"
#include "MainWindow.h"

#ifdef Q_OS_WIN
static bool handleException(const wchar_t *dumpPath,
                            const wchar_t *minidumpId,
                            void *context,
                            EXCEPTION_POINTERS *exinfo,
                            MDRawAssertionInfo *assertion,
                            bool succeeded)
{
    return true;
}
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    GlobalConfig::create();

    // Set crash handler
#ifdef Q_OS_WIN
    QScopedPointer<google_breakpad::ExceptionHandler> exHandler(new google_breakpad::ExceptionHandler(
        GlobalConfig::instance()->crashDumpPath().toStdWString(),
        nullptr,
        handleException,
        nullptr,
        google_breakpad::ExceptionHandler::HANDLER_ALL
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
