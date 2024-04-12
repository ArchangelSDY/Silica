#include "SaveToSharer.h"

#include <QFileDialog>
#include <QSharedPointer>

#include "image/Image.h"
#include "image/ImageSource.h"
#include "GlobalConfig.h"

bool SaveToSharer::share(QSharedPointer<Image> image)
{
    QString destDir = QFileDialog::getExistingDirectory(
        nullptr, "Save to", GlobalConfig::instance()->wallpaperDir());

    if (destDir.isEmpty()) {
        return false;
    }

    QFileInfo imageFile(image->name()); // Remove dir in image name
    QString destPath = destDir + "/" +
        imageFile.fileName();
    return image->source()->copy(destPath);
}
