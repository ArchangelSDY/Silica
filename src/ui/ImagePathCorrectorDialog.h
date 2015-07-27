#ifndef IMAGEPATHCORRECTORDIALOG_H
#define IMAGEPATHCORRECTORDIALOG_H

#include <QDialog>

#include "logger/listeners/ImagePathCorrector.h"

namespace Ui {
class ImagePathCorrectorDialog;
}

class QListWidgetItem;

class ImagePathCorrectorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImagePathCorrectorDialog(QList<ImagePathCorrector::PathPatch> &patches,
                                      QWidget *parent = 0);
    ~ImagePathCorrectorDialog();

private slots:
    void onItemChanged(QListWidgetItem *item);

private:
    Ui::ImagePathCorrectorDialog *ui;
    QList<ImagePathCorrector::PathPatch> &m_patches;
};

#endif // IMAGEPATHCORRECTORDIALOG_H
