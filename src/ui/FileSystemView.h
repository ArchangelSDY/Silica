#ifndef FILESYSTEMVIEW_H
#define FILESYSTEMVIEW_H

#include <QFileSystemWatcher>
#include <QStringList>

#include "ui/GalleryView.h"

class FileSystemView : public GalleryView
{
    Q_OBJECT
public:
    explicit FileSystemView(QWidget *parent = 0);
    ~FileSystemView();

    QString rootPath() const;

signals:
    void rootPathChanged(const QString &path);

public slots:
    void setRootPath(const QString &path);
    void cdUp();

protected slots:
    void sortByFlag(QDir::SortFlag flag);
    void sortByName();
    void sortByModifiedTime();

    void refreshView();
    void dirIterGotItem(const QString &rootPath, const QString &itemAbsPath);
    void removeSelectedOnDisk();

protected:
    virtual void contextMenuEvent(QContextMenuEvent *event);

private:
    QString m_rootPath;
    QFileSystemWatcher m_pathWatcher;
    QStringList m_entries;
    QDir::SortFlags m_sortFlags;

    class DirIterThread;
    DirIterThread *m_dirIterThread;
};

#endif // FILESYSTEMVIEW_H
