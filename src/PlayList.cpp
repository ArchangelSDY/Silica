#include <QDir>
#include <quazip.h>

#include "PlayList.h"

PlayList::PlayList(const QList<QUrl> &imageUrls)
{
    foreach(const QUrl &imageUrl, imageUrls) {
        addPath(imageUrl);
    }
}

void PlayList::addPath(const QString &path)
{
    QFileInfo file(path);

    if (!file.exists()) {
        return;
    }

    if (file.isDir()) {
        QDir dir(path);

        QStringList filters;
        filters << "*.png" << "*.jpg";
        dir.setNameFilters(filters);

        if (dir.entryInfoList().length() > 0) {
            foreach (const QFileInfo& fileInfo, dir.entryInfoList()) {
                QUrl imageUrl = QUrl::fromLocalFile(
                    fileInfo.absoluteFilePath());
                *this << QSharedPointer<Image>(new Image(imageUrl));
            }
        }
    } else {
        *this << QSharedPointer<Image>(new Image(QUrl::fromLocalFile(path)));
    }
}

void PlayList::addPath(const QUrl &url)
{
    if (url.isEmpty()) {
        return;
    }

    if (url.scheme() == "file") {
        QString path = url.toLocalFile();
        addPath(path);
    } else if (url.scheme() == "zip") {
        if (url.hasFragment()) {
            // Single file
            *this << QSharedPointer<Image>(new Image(url));
        } else {
            // Add all files in the zip
            // FIXME: Should skip non image files
            QUrl fileUrl = url;
            fileUrl.setScheme("file");
            QString zipPath = fileUrl.toLocalFile();

            QuaZip zip(zipPath);
            bool success = zip.open(QuaZip::mdUnzip);

            if (success) {
                foreach(const QString &name, zip.getFileNameList()) {
                    QUrl imageUrl = url;
                    imageUrl.setFragment(name);

                    *this << QSharedPointer<Image>(new Image(imageUrl));
                }

                zip.close();
            }

        }
    }
}

static bool imageNameLessThan(const QSharedPointer<Image> &left,
                              const QSharedPointer<Image> &right)
{
    return left->name() < right->name();
}

void PlayList::sortByName()
{
    qSort(this->begin(), this->end(), imageNameLessThan);
}

static bool imageAspectRatioLessThan(const QSharedPointer<Image> &left,
                                     const QSharedPointer<Image> &right)
{
    return left->aspectRatio() < right->aspectRatio();
}

void PlayList::sortByAspectRatio()
{
    qSort(this->begin(), this->end(), imageAspectRatioLessThan);
}
