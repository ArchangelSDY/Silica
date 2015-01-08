#include <QDir>
#include <QFileInfo>
#include <QMenu>

#include "image/ImageSourceManager.h"
#include "ui/FileSystemItem.h"
#include "ui/FileSystemView.h"
#include "ui/renderers/CompactRendererFactory.h"

FileSystemView::FileSystemView(QWidget *parent) :
    GalleryView(parent)
{
    m_rendererFactory = new CompactRendererFactory();

    connect(this, SIGNAL(keyEnterPressed()), this, SLOT(triggerOpenDir()));
}

void FileSystemView::setRootPath(const QString &path)
{
    clear();

    QDir dir(path);
    QFileInfoList entrieInfos = dir.entryInfoList(
        ImageSourceManager::instance()->nameFilters(),
        QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot,
        QDir::Name | QDir::DirsFirst);

    foreach (const QFileInfo &info, entrieInfos) {
        FileSystemItem *item = new FileSystemItem(info.absoluteFilePath(),
                                                  m_rendererFactory);
        addItem(item);
    }

    scheduleLayout();
}

void FileSystemView::triggerOpenDir()
{
    QList<QGraphicsItem *> selectedItems = scene()->selectedItems();
    if (!selectedItems.isEmpty()) {
        FileSystemItem *firstItem =
            static_cast<FileSystemItem *>(selectedItems[0]);
        emit openDir(firstItem->path());
    }
}

void FileSystemView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = new QMenu(this);

    QList<QGraphicsItem *> selectedItems = scene()->selectedItems();
    if (!selectedItems.isEmpty()) {
        FileSystemItem *firstItem =
            static_cast<FileSystemItem *>(selectedItems[0]);
        if (firstItem->fileInfo().isDir()) {
            menu->addAction(tr("Open Directory"), this, SLOT(triggerOpenDir()));
        }
    }

    menu->exec(event->globalPos());
}
