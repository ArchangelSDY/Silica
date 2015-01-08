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
