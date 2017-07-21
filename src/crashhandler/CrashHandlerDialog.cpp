#include "CrashHandlerDialog.h"
#include "ui_CrashHandlerDialog.h"

#include <QFile>
#include <QHttpMultipart>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QProcess>

CrashHandlerDialog::CrashHandlerDialog(const QString &uploadUrl,
                                       const quint32 &crashId,
                                       const QString &dumpFile,
                                       const QStringList &clientStartupArguments,
                                       QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CrashHandlerDialog) , 
    m_uploadUrl(uploadUrl) ,
    m_crashId(crashId) ,
    m_dumpFile(dumpFile) ,
    m_clientStartupArguments(clientStartupArguments)
{
    ui->setupUi(this);
    ui->progressUpload->hide();
    ui->chkUpload->setVisible(m_uploadUrl.isValid());
    ui->chkUpload->setChecked(m_uploadUrl.isValid());

    QUrl dumpFileUrl = QUrl::fromLocalFile(m_dumpFile);
    QString message = ui->lblTitle->text()
        .arg(dumpFileUrl.toString())
        .arg(m_dumpFile);
    ui->lblTitle->setText(message);

    connect(ui->btnQuit, &QPushButton::clicked, this, &CrashHandlerDialog::onQuit);
    connect(ui->btnRestart, &QPushButton::clicked, this, &CrashHandlerDialog::onRestart);
}

CrashHandlerDialog::~CrashHandlerDialog()
{
    delete ui;
}

void CrashHandlerDialog::onRestart()
{
    doQuit(ui->chkUpload->isChecked(), true);
}

void CrashHandlerDialog::onQuit()
{
    doQuit(ui->chkUpload->isChecked(), false);
}

void CrashHandlerDialog::doQuit(bool shouldUpload, bool shouldRestart)
{
    if (shouldUpload && m_uploadUrl.isValid()) {
        uploadCrashReport();
    }

    if (shouldRestart && !m_clientStartupArguments.isEmpty()) {
        QString command = m_clientStartupArguments[0];
        QStringList args;
        if (m_clientStartupArguments.size() > 1) {
            args = m_clientStartupArguments.mid(1);
        }

        QProcess::startDetached(command, args);
    }

    QApplication::exit();
}

void CrashHandlerDialog::uploadCrashReport()
{
    ui->progressUpload->show();

    QHttpMultiPart *multipart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart productPart;
    productPart.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain");
    productPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"product\""));
    productPart.setBody("Silica");
    multipart->append(productPart);

    QHttpPart crashIdPart;
    crashIdPart.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain");
    crashIdPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"crash_id\""));
    crashIdPart.setBody(QByteArray::number(m_crashId));
    multipart->append(crashIdPart);

    QFile *dumpFile = new QFile(m_dumpFile, multipart);
    QHttpPart dumpPart;
    dumpPart.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");
    dumpPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"dump_file\"; filename=\""+ dumpFile->fileName() + "\""));
    if (!dumpFile->open(QIODevice::ReadOnly)) {
        this->accept();
        return;
    }
    dumpPart.setBodyDevice(dumpFile);
    multipart->append(dumpPart);
    ui->progressUpload->setMaximum(dumpFile->size());

    QNetworkRequest req(m_uploadUrl);
    QNetworkReply *reply = m_netMgr.post(req, multipart);
    multipart->setParent(reply);

    connect(reply, &QNetworkReply::uploadProgress, [this](qint64 bytesSent, qint64 bytesTotal) {
        this->ui->progressUpload->setValue(bytesSent);
    });

    connect(reply, &QNetworkReply::finished, [this, reply]() {
        this->ui->progressUpload->setValue(this->ui->progressUpload->maximum());
        this->accept();
    });
}
