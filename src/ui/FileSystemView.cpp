#include <QDir>
#include <QFileInfo>
#include <QMenu>
#include <QMessageBox>

#include "image/ImageSourceManager.h"
#include "ui/FileSystemItem.h"
#include "ui/FileSystemView.h"
#include "ui/renderers/CompactRendererFactory.h"

FileSystemView::FileSystemView(QWidget *parent) :
    GalleryView(parent) ,
    m_sortFlags(QDir::Name | QDir::DirsFirst)
{
    setRendererFactory(new CompactRendererFactory());
    connect(&m_pathWatcher, SIGNAL(directoryChanged(QString)),
            this, SLOT(refreshView()));
}

void FileSystemView::setRootPath(const QString &path)
{
    if (m_rootPath == path) {
        return;
    }
    if (!m_rootPath.isEmpty()) {
        m_pathWatcher.removePath(m_rootPath);
    }
    m_rootPath = path;
    m_pathWatcher.addPath(m_rootPath);

    refreshView();

    // Reset scroll position
    centerOn(0, 0);

    leaveSearch();

    emit rootPathChanged(m_rootPath);
}

void FileSystemView::cdUp()
{
    QDir dir(m_rootPath);
    dir.cdUp();
    setRootPath(dir.absolutePath());
}

void FileSystemView::sortByFlag(QDir::SortFlag flag)
{
    // Cannot use QFlags::testFlag() here because Qt sets QDir::Name == 0x00
    // so that testFlag() will fail.
    bool isReversed = ((m_sortFlags & flag) == flag)
        && ((m_sortFlags & QDir::Reversed) == QDir::Reversed);
    if (isReversed) {
        m_sortFlags = flag | QDir::DirsFirst;
    } else {
        m_sortFlags = flag | QDir::DirsFirst | QDir::Reversed;
    }

    refreshView();
}

void FileSystemView::sortByName()
{
    sortByFlag(QDir::Name);
}

void FileSystemView::sortByModifiedTime()
{
    sortByFlag(QDir::Time);
}

void FileSystemView::refreshView()
{
    clear();

    QDir dir(m_rootPath);
    QFileInfoList entrieInfos = dir.entryInfoList(
        ImageSourceManager::instance()->nameFilters(),
        QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot,
        m_sortFlags);

    incrItemsToLoad(entrieInfos.count());

    foreach (const QFileInfo &info, entrieInfos) {
        FileSystemItem *item = new FileSystemItem(info.absoluteFilePath(),
                                                  rendererFactory());
        addItem(item);
    }

    scheduleLayout();
}

void FileSystemView::removeSelectedOnDisk()
{
    QList<GalleryItem *> selectedItems = selectedGalleryItems();

    QString msg = tr("Remove %1 files on disk?").arg(selectedItems.count());
    if (QMessageBox::question(
            this, tr("Remove On Disk"), msg) == QMessageBox::Yes) {
        foreach (GalleryItem *item, selectedItems) {
            FileSystemItem *fsItem =
                static_cast<FileSystemItem *>(item);
            fsItem->removeOnDisk();
        }
    }

    refreshView();
}

void FileSystemView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = new QMenu(this);

    QMenu *menuSort = menu->addMenu(tr("Sort By"));
    QActionGroup *grpSorts = new QActionGroup(menu);
    QAction *actSortByName = menuSort->addAction(tr("Name"),
        this, SLOT(sortByName()));
    grpSorts->addAction(actSortByName);
    actSortByName->setCheckable(true);
    QAction *actSortByModified = menuSort->addAction(tr("Last Modified"),
        this, SLOT(sortByModifiedTime()));
    grpSorts->addAction(actSortByModified);
    actSortByModified->setCheckable(true);
    actSortByModified->setChecked((m_sortFlags & QDir::Time) == QDir::Time);
    // Hack here because Qt sets QDir::Name == 0x00.....WTF
    if (grpSorts->checkedAction() == 0) {
        actSortByName->setChecked(true);
    }

    menu->addSeparator();

    QList<GalleryItem *> selectedItems = selectedGalleryItems();
    if (selectedItems.count() > 0) {
        menu->addAction(tr("Remove On Disk"), this,
                        SLOT(removeSelectedOnDisk()));
    }

    menu->exec(event->globalPos());
    connect(menu, SIGNAL(aboutToHide()), menu, SLOT(deleteLater()));
}
