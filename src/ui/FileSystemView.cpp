#include <QDir>
#include <QFileInfo>
#include <QMenu>
#include <QMessageBox>

#include "image/ImageSourceManager.h"
#include "ui/FileSystemItem.h"
#include "ui/FileSystemView.h"
#include "ui/renderers/CompactRendererFactory.h"

FileSystemView::FileSystemView(QWidget *parent) :
    GalleryView(parent)
{
    m_rendererFactory = new CompactRendererFactory();
}

void FileSystemView::setRootPath(const QString &path)
{
    clear();

    m_rootPath = path;
    QDir dir(m_rootPath);
    QFileInfoList entrieInfos = dir.entryInfoList(
        ImageSourceManager::instance()->nameFilters(),
        QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot,
        QDir::Name | QDir::DirsFirst);

    incrItemsToLoad(entrieInfos.count());

    foreach (const QFileInfo &info, entrieInfos) {
        FileSystemItem *item = new FileSystemItem(info.absoluteFilePath(),
                                                  m_rendererFactory);
        addItem(item);
    }

    scheduleLayout();

    // Reset scroll position
    centerOn(0, 0);
}

void FileSystemView::cdUp()
{
    QDir dir(m_rootPath);
    dir.cdUp();
    setRootPath(dir.absolutePath());
}

void FileSystemView::removeSelectedOnDisk()
{
    QList<QGraphicsItem *> selectedItems = scene()->selectedItems();

    QString msg = tr("Remove %1 files on disk?").arg(selectedItems.count());
    if (QMessageBox::question(
            this, tr("Remove On Disk"), msg) == QMessageBox::Yes) {
        foreach (QGraphicsItem *item, selectedItems) {
            FileSystemItem *fsItem =
                static_cast<FileSystemItem *>(item);
            QFile::remove(fsItem->path());
        }
    }

    // Refresh
    setRootPath(m_rootPath);
}

void FileSystemView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = new QMenu(this);

    QList<QGraphicsItem *> selectedItems = scene()->selectedItems();
    if (selectedItems.count() > 0) {
        menu->addAction(tr("Remove On Disk"), this,
                        SLOT(removeSelectedOnDisk()));
    }

    menu->exec(event->globalPos());
}
