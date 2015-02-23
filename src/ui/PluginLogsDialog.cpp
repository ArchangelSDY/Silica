#include "PluginLogsDialog.h"
#include "ui_PluginLogsDialog.h"

#include "sapi/PluginLoggingDelegate.h"

PluginLogsDialog::PluginLogsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PluginLogsDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::WindowStaysOnTopHint);
    if (m_buf.open(QIODevice::ReadWrite)) {
        sapi::PluginLoggingDelegate::instance()->setDevice(&m_buf);
        connect(&m_buf, SIGNAL(bytesWritten(qint64)),
                this, SLOT(updateLogs()));
    }
}

PluginLogsDialog::~PluginLogsDialog()
{
    sapi::PluginLoggingDelegate::instance()->setDevice(0);
    delete ui;
}

void PluginLogsDialog::updateLogs()
{
    ui->txtLogs->setText(QString(m_buf.data()));
}
