#include <QStringListModel>
#include <QScrollBar>

#include "FixedRegionConfDialog.h"
#include "ui_FixedRegionConfDialog.h"

FixedRegionConfDialog::FixedRegionConfDialog(
        QList<qreal> &centers,
        QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FixedRegionConfDialog) ,
    m_centers(centers) ,
    m_centersModel(0)
{
    ui->setupUi(this);
    connect(ui->btnAdd, SIGNAL(clicked()), this, SLOT(addCenter()));
    connect(ui->btnRemove, SIGNAL(clicked()), this, SLOT(removeSelected()));
}

FixedRegionConfDialog::~FixedRegionConfDialog()
{
    delete ui;
    if (m_centersModel) {
        m_centersModel->deleteLater();
    }
}

void FixedRegionConfDialog::showEvent(QShowEvent *)
{
    resetModel();
}

void FixedRegionConfDialog::resetModel()
{
    if (m_centersModel) {
        m_centersModel->deleteLater();
    }

    QStringList modelList;
    foreach (const qreal &center, m_centers) {
        modelList << QString("%1%").arg(center * 100);
    }

    m_centersModel = new QStringListModel(modelList);
    ui->lstCenters->setModel(m_centersModel);

    ui->btnRemove->setEnabled(!m_centers.isEmpty());
}

void FixedRegionConfDialog::addCenter()
{
    qreal center = ui->spnCenter->value() / 100.0;
    m_centers.append(center);
    qSort(m_centers.begin(), m_centers.end());
    resetModel();
}

void FixedRegionConfDialog::removeSelected()
{
    int row = ui->lstCenters->selectionModel()->currentIndex().row();
    m_centers.removeAt(row);
    ui->lstCenters->model()->removeRow(row);
    ui->btnRemove->setEnabled(!m_centers.isEmpty());
}
