#ifndef PLUGINLOGSDIALOG_H
#define PLUGINLOGSDIALOG_H

#include <QBuffer>
#include <QDialog>

namespace Ui {
class PluginLogsDialog;
}

class PluginLogsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PluginLogsDialog(QWidget *parent = 0);
    ~PluginLogsDialog();

private slots:
    void updateLogs();

private:
    Ui::PluginLogsDialog *ui;
    QBuffer m_buf;
};

#endif // PLUGINLOGSDIALOG_H
