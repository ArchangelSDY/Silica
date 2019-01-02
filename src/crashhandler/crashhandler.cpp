#include <iostream>
#include <QApplication>
#include <QTimer>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#include "deps/breakpad/client/windows/crash_generation/crash_generation_server.h"
#include "deps/breakpad/client/windows/crash_generation/client_info.h"

#include "crashhandler/CrashHandlerDialog.h"

class CrashHandler : public QObject
{
    Q_OBJECT
public:

    CrashHandler(const QString &uploadHost, const QStringList &clientStartupArguments)
        : m_uploadHost(uploadHost)
        , m_clientStartupArguments(clientStartupArguments)
    {
        QObject::connect(this, &CrashHandler::emitOnCrash, this, &CrashHandler::showDialog);
    }

signals:
    void emitOnCrash(const google_breakpad::ClientInfo *info, const QString &dumpPath);

private slots:
    void showDialog(const google_breakpad::ClientInfo *info, const QString &dumpPath)
    {
        CrashHandlerDialog dialog(
            m_uploadHost,
            info->crash_id(),
            dumpPath,
            m_clientStartupArguments
        );
        dialog.exec();
    }

private:
    QString m_uploadHost;
    QStringList m_clientStartupArguments;
};

static void onCrash(void *context, const google_breakpad::ClientInfo *info, const std::wstring *dumpPath)
{
    CrashHandler *handler = reinterpret_cast<CrashHandler *>(context);
    emit handler->emitOnCrash(info, QString::fromStdWString(*dumpPath));
}

static void checkParentLiveness(qint64 pid)
{
#ifdef Q_OS_WIN
    HANDLE handle = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, false, pid);
    if (handle != NULL) {
        DWORD parentExitCode = 0;
        if (GetExitCodeProcess(handle, &parentExitCode) && parentExitCode == STILL_ACTIVE) {
            CloseHandle(handle);
            return;
        }
    }

    qApp->exit();
#endif
}

// crashhandler.exe <pipe_name> <dump_path> <upload_host> <parent_pid> <client_argument>...
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QStringList args = a.arguments();
    if (args.size() < 5) {
        qWarning("Invalid arguments");
        return 1;
    }

    std::wstring pipeName = args[1].toStdWString();
    std::wstring dumpPath = args[2].toStdWString();

    QString uploadHost = args[3];
    QStringList clientStartupArguments;
    if (args.size() > 5) {
        clientStartupArguments = args.mid(5);
    }

    CrashHandler handler(uploadHost, clientStartupArguments);
    google_breakpad::CrashGenerationServer server(
        pipeName,
        nullptr,
        nullptr, // OnConnected
        nullptr,
        onCrash, // OnDumpRequest
        &handler,
        nullptr, // OnExited
        nullptr,
        nullptr, // OnUploadRequest
        nullptr,
        true,
        &dumpPath
    );

    bool status = server.Start();

    std::cout << (status ? '0' : '1') << std::flush;

    QTimer livenessTimer;
    livenessTimer.setInterval(5000);
    livenessTimer.setSingleShot(false);
    QObject::connect(&handler, &CrashHandler::emitOnCrash, &livenessTimer, &QTimer::stop);
    QObject::connect(&livenessTimer, &QTimer::timeout, [pid = args[4].toInt()]() {
        checkParentLiveness(pid);
    });
    livenessTimer.start();

    return a.exec();
}

#include "crashhandler.moc"