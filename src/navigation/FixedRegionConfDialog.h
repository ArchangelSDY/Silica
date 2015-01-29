#ifndef FIXEDREGIONCONFDIALOG_H
#define FIXEDREGIONCONFDIALOG_H

#include <QDialog>

#include "navigation/FixedRegionNavigationPlayer.h"

namespace Ui {
class FixedRegionConfDialog;
}

class QItemSelection;
class QStringListModel;

class FixedRegionConfDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FixedRegionConfDialog(
        QList<qreal> &centers,
        QWidget *parent = 0);
    ~FixedRegionConfDialog();

protected:
    void showEvent(QShowEvent *);

private slots:
    void resetModel();
    void addCenter();
    void removeSelected();

private:
    Ui::FixedRegionConfDialog *ui;
    QList<qreal> &m_centers;
    QStringListModel *m_centersModel;
};

#endif // FIXEDREGIONCONFDIALOG_H
