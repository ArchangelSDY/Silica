#include <QDir>

#include "PlayList.h"

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
                *this << QUrl::fromLocalFile(fileInfo.absoluteFilePath());
            }
        }
    } else {
        *this << QUrl::fromLocalFile(path);
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
        *this << url;
    }
}
