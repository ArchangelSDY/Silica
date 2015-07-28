#include "ImagePathCorrectorDialog.h"
#include "ui_ImagePathCorrectorDialog.h"

#include <QListWidgetItem>

ImagePathCorrectorDialog::ImagePathCorrectorDialog(
        QList<ImagePathCorrector::PathPatch> &patches,
        QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImagePathCorrectorDialog) ,
    m_patches(patches)
{
    ui->setupUi(this);

    for (int i = 0; i < m_patches.count(); ++i) {
        const ImagePathCorrector::PathPatch &patch = m_patches[i];
        QString showText= patch.toString();

        QListWidgetItem *item = new QListWidgetItem(showText, ui->lstPatches);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Checked);
        item->setData(Qt::EditRole, i);
        item->setData(Qt::DisplayRole, showText);
        ui->lstPatches->addItem(item);
    }

    connect(ui->lstPatches, SIGNAL(itemChanged(QListWidgetItem*)),
            this, SLOT(onItemChanged(QListWidgetItem*)));
}

ImagePathCorrectorDialog::~ImagePathCorrectorDialog()
{
    delete ui;
}

void ImagePathCorrectorDialog::onItemChanged(QListWidgetItem *item)
{
    int index = item->data(Qt::EditRole).toInt();
    m_patches[index].shouldApply = (item->checkState() == Qt::Checked);
}
