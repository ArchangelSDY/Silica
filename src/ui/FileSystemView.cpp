#include <QDir>
#include <QFileInfo>
#include <QMenu>
#include <QMessageBox>
#include <QThread>

#include "image/ImageSourceManager.h"
#include "ui/FileSystemItem.h"
#include "ui/FileSystemView.h"
#include "ui/renderers/CompactRendererFactory.h"

class FileSystemView::DirIterThread : public QThread
{
    Q_OBJECT
public:
    DirIterThread() : m_shouldQuit(false)
    {
    }

    void setRootPath(const QString &rootPath)
    {
        m_rootPath = rootPath;
    }

    void setNameFilters(const QStringList &nameFilters)
    {
        m_nameFilters = nameFilters;
    }

    void setFilters(QDir::Filters filters)
    {
        m_filters = filters;
    }

    void run() override
    {
        QDirIterator iter(m_rootPath, m_nameFilters, m_filters);
        while (iter.hasNext()) {
            emit gotItem(m_rootPath, iter.next());
            if (m_shouldQuit) {
                break;
            }
        }
    }

    void reset()
    {
        m_shouldQuit = false;
    }

    void stopIter()
    {
        disconnect(this, SIGNAL(gotItem(QString, QString)), 0, 0);
        disconnect(this, SIGNAL(finished()), 0, 0);
        m_shouldQuit = true;
    }

signals:
    void gotItem(const QString &rootPath, const QString &itemAbsPath);

private:
    QString m_rootPath;
    QStringList m_nameFilters;
    QDir::Filters m_filters;
    bool m_shouldQuit;
};

FileSystemView::FileSystemView(QWidget *parent) :
    GalleryView(parent) ,
    m_sortFlags(QDir::Name | QDir::DirsFirst) ,
    m_dirIterThread(new DirIterThread())
{
    setRendererFactory(new CompactRendererFactory());
    connect(&m_pathWatcher, SIGNAL(directoryChanged(QString)),
            this, SLOT(refreshView()));
}

FileSystemView::~FileSystemView()
{
    if (m_dirIterThread->isRunning()) {
        m_dirIterThread->stopIter();
        m_dirIterThread->wait();
    }
    m_dirIterThread->deleteLater();
}

QString FileSystemView::rootPath() const
{
    return m_rootPath;
}

void FileSystemView::setRootPath(const QString &path)
{
    if (m_rootPath == path) {
        return;
    }
    m_rootPath = path;

    if (m_dirIterThread->isRunning()) {
        m_dirIterThread->stopIter();
        m_dirIterThread->wait();
    }

    if (!m_rootPath.isEmpty()) {
        m_pathWatcher.removePath(m_rootPath);
    }
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
    if (m_dirIterThread->isRunning()) {
        return;
    }

    clear();

    m_dirIterThread->reset();
    m_dirIterThread->setRootPath(m_rootPath);
    m_dirIterThread->setNameFilters(
        ImageSourceManager::instance()->nameFilters());
    m_dirIterThread->setFilters(
        QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);

    connect(m_dirIterThread, SIGNAL(gotItem(QString, QString)),
            this, SLOT(dirIterGotItem(QString, QString)),
            static_cast<Qt::ConnectionType>(
                Qt::AutoConnection | Qt::UniqueConnection));

    m_dirIterThread->start();
}

void FileSystemView::dirIterGotItem(const QString &rootPath,
                                    const QString &itemAbsPath)
{
    if (rootPath == m_rootPath) {
        incrItemsToLoad(1);
        FileSystemItem *item = new FileSystemItem(itemAbsPath,
                                                  rendererFactory());
        addItem(item);

        scheduleLayout();
    }
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


#include "FileSystemView.moc"
