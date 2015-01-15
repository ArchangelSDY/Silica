#ifndef FILESYSTEMVIEW_H
#define FILESYSTEMVIEW_H

#include <QStringList>

#include "ui/GalleryView.h"

class FileSystemView : public GalleryView
{
    Q_OBJECT
public:
    explicit FileSystemView(QWidget *parent = 0);

public slots:
    void setRootPath(const QString &path);
    void cdUp();

protected slots:
    void sortByFlag(QDir::SortFlag flag);
    void sortByName();
    void sortByModifiedTime();

    void removeSelectedOnDisk();

protected:
    virtual void contextMenuEvent(QContextMenuEvent *event);

private:
    QString m_rootPath;
    QStringList m_entries;
    QDir::SortFlags m_sortFlags;
};

#endif // FILESYSTEMVIEW_H