#include "SaveToSharer.h"

#include <QFileDialog>
#include <QSharedPointer>

#include "image/Image.h"
#include "image/ImageSource.h"
#include "GlobalConfig.h"

bool SaveToSharer::share(const QList<QSharedPointer<Image>> &images)
{
    QString destDir = QFileDialog::getExistingDirectory(
        nullptr, "Save to", GlobalConfig::instance()->wallpaperDir());

    if (destDir.isEmpty()) {
        return false;
    }

    for (auto image : images) {
        QFileInfo imageFile(image->name()); // Remove dir in image name
        QString destPath = destDir + "/" +
            imageFile.fileName();
        if (!image->source()->copy(destPath)) {
            return false;
        }
    }

    return true;
}
