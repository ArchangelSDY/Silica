#ifndef CRASHHANDLERDIALOG_H
#define CRASHHANDLERDIALOG_H

#include <QDialog>
#include <QNetworkAccessManager>
#include <QUrl>

namespace Ui {
class CrashHandlerDialog;
}

class CrashHandlerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CrashHandlerDialog(const QString &uploadUrl,
                                const quint32 &crashId,
                                const QString &dumpFile,
                                const QStringList &clientStartupArguments,
                                QWidget *parent = 0);

    ~CrashHandlerDialog();

private slots:
    void onRestart();
    void onQuit();
    void doQuit(bool shouldUpload, bool shouldRestart);

private:
    void uploadCrashReport();

    Ui::CrashHandlerDialog *ui;
    QNetworkAccessManager m_netMgr;
    QUrl m_uploadUrl;
    quint32 m_crashId;
    QString m_dumpFile;
    QStringList m_clientStartupArguments;
};

#endif // CRASHHANDLERDIALOG_H
