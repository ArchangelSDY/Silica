#ifndef FILESYSTEMVIEW_H
#define FILESYSTEMVIEW_H

#include <QStringList>

#include "ui/GalleryView.h"

class FileSystemView : public GalleryView
{
    Q_OBJECT
public:
    explicit FileSystemView(QWidget *parent = 0);

signals:
    void openDir(QString dir);

public slots:
    void setRootPath(const QString &path);

private slots:
    void triggerOpenDir();

protected:
    virtual void contextMenuEvent(QContextMenuEvent *event);

private:
    QStringList m_entries;
};

#endif // FILESYSTEMVIEW_H
