#include "ui/FileSystemView.h"

#include <QDir>
#include <QFileInfo>
#include <QMenu>
#include <QMessageBox>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QScrollBar>
#include <QThread>

#ifdef ENABLE_OPENGL
#include <QGL>
#include <QOpenGLWidget>
#endif

#include "image/ImageSourceManager.h"
#include "ui/FileSystemItem.h"
#include "ui/renderers/CompactRendererFactory.h"

class ItemSortComparator
{
public:
    ItemSortComparator(QDir::SortFlags sortFlags) :
        m_sortFlags(sortFlags)
    {
    }

    bool operator()(const QFileInfo &left, const QFileInfo &right) const
    {
        int ret = 0;
        if (left.isDir() == right.isDir()) {
            int sortBy = (m_sortFlags & QDir::SortByMask)
                | (m_sortFlags & QDir::Type);
            switch (sortBy) {
            case QDir::Time: {
                // Sort by time
                QDateTime lt = left.lastModified();
                QDateTime rt = right.lastModified();

                lt.setTimeSpec(Qt::UTC);
                rt.setTimeSpec(Qt::UTC);

                ret = lt.secsTo(rt);
                break;
            }
            default:
                break;
            }

            if (ret == 0 && sortBy != QDir::Unsorted) {
                // Still not sorted, sort by name
                bool ignoreCase = (m_sortFlags & QDir::IgnoreCase);
                QString leftName = ignoreCase ?
                    left.fileName().toLower() :
                    left.fileName();
                QString rightName = ignoreCase ?
                    right.fileName().toLower() :
                    right.fileName();

                if (m_sortFlags & QDir::LocaleAware) {
                    ret = leftName.compare(rightName);
                } else {
                    ret = leftName.localeAwareCompare(rightName);
                }
            }
        } else if (left.isDir()) {
            // Dir, File
            // Always dirs first
            ret = true;
        } else {
            // File, Dir
            // Always dirs first
            ret = false;
        }

        bool isReversed = (m_sortFlags & QDir::Reversed);
        return !isReversed ? ret < 0 : ret > 0;
    }

    bool operator()(const QGraphicsItem *left, const QGraphicsItem *right) const
    {
        const FileSystemItem *leftItem =
            static_cast<const FileSystemItem *>(left);
        const FileSystemItem *rightItem =
            static_cast<const FileSystemItem *>(right);
        QFileInfo leftInfo = leftItem->fileInfo();
        QFileInfo rightInfo = rightItem->fileInfo();

        return operator ()(leftInfo, rightInfo);
    }

private:
    QDir::SortFlags m_sortFlags;
};


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

    void setSortFlags(QDir::SortFlags sortFlags)
    {
        m_sortFlags = sortFlags;
    }

    void run() override
    {
        QDirIterator iter(m_rootPath, m_nameFilters, m_filters);
        QList<QFileInfo> itemInfos;
        bool needSort = ((m_sortFlags & QDir::SortByMask) != QDir::Unsorted);

        while (iter.hasNext()) {
            // Emit item at once if sorting is not needed, otherwise hold and
            // sort after all items found
            if (!needSort) {
                emit gotItem(m_rootPath, iter.next());
            } else {
                iter.next();
                itemInfos.append(iter.fileInfo());
            }

            if (m_shouldQuit) {
                break;
            }
        }

        if (needSort) {
            std::sort(itemInfos.begin(), itemInfos.end(),
                      ItemSortComparator(m_sortFlags));
            foreach (const QFileInfo &info, itemInfos) {
                emit gotItem(m_rootPath, info.absoluteFilePath());
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
    QDir::SortFlags m_sortFlags;
    bool m_shouldQuit;
};


FileSystemView::FileSystemView(QWidget *parent) :
    GalleryView(parent) ,
    m_sortFlags(QDir::Unsorted) ,
    m_isFirstRefreshAfterRootPathChanged(true) ,
    m_dirIterThread(new DirIterThread())
{
#ifdef ENABLE_OPENGL
    m_view->setViewport(new QOpenGLWidget(this));
#endif

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

    // Save scroll position
    int hs = m_view->horizontalScrollBar()->value();
    int vs = m_view->verticalScrollBar()->value();
    m_historyScrollPositions.insert(m_rootPath, QPoint(hs, vs));

    // Reset scroll position
    m_view->horizontalScrollBar()->setValue(0);
    m_view->verticalScrollBar()->setValue(0);

    m_rootPath = path;
    m_isFirstRefreshAfterRootPathChanged = true;

    if (m_dirIterThread->isRunning()) {
        m_dirIterThread->stopIter();
        m_dirIterThread->wait();
    }

    if (!m_rootPath.isEmpty()) {
        m_pathWatcher.removePath(m_rootPath);
    }
    // Path watcher will be added after dir listing finished.

    refreshView();

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
    int sortTypes = m_sortFlags & QDir::SortByMask;
    if (sortTypes & flag) {
        // Flag not change
        bool isReversed = (m_sortFlags & QDir::Reversed);
        if (!isReversed) {
            m_sortFlags = flag | QDir::DirsFirst | QDir::Reversed;
        } else {
            m_sortFlags = flag | QDir::DirsFirst;
        }
    } else {
        // Flag change
        m_sortFlags = flag | QDir::DirsFirst;
    }

    refreshView();
}

void FileSystemView::sortByNothing()
{
    sortByFlag(QDir::NoSort);
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
    m_dirIterThread->setSortFlags(m_sortFlags);

    connect(m_dirIterThread, SIGNAL(gotItem(QString, QString)),
            this, SLOT(dirIterGotItem(QString, QString)),
            static_cast<Qt::ConnectionType>(
                Qt::AutoConnection | Qt::UniqueConnection));
    connect(m_dirIterThread, SIGNAL(finished()),
            this, SLOT(dirIterFinished()),
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

void FileSystemView::dirIterFinished()
{
    if (m_isFirstRefreshAfterRootPathChanged) {
        m_isFirstRefreshAfterRootPathChanged = false;

        // Add to path watcher
        m_pathWatcher.addPath(m_rootPath);

        // Restore scroll position
        QPoint lastScrollPos = m_historyScrollPositions.value(m_rootPath);
        scrollToPositionWithAnimation(lastScrollPos);
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

    // Disable sort if dir listing or sort is in progress
    if (m_dirIterThread->isRunning()) {
        menuSort->setDisabled(true);
        menuSort->setTitle(tr("Sort By (In progress...)"));
    } else {
        int sortType = m_sortFlags & QDir::SortByMask;
        QActionGroup *grpSorts = new QActionGroup(menu);
        QAction *actSortByNothing = menuSort->addAction(tr("No Sort"),
            this, SLOT(sortByNothing()));
        grpSorts->addAction(actSortByNothing);
        actSortByNothing->setCheckable(true);
        actSortByNothing->setChecked(sortType == QDir::Unsorted);

        QAction *actSortByName = menuSort->addAction(tr("Name"),
            this, SLOT(sortByName()));
        grpSorts->addAction(actSortByName);
        actSortByName->setCheckable(true);
        actSortByName->setChecked(sortType == QDir::Name);

        QAction *actSortByModified = menuSort->addAction(tr("Last Modified"),
            this, SLOT(sortByModifiedTime()));
        grpSorts->addAction(actSortByModified);
        actSortByModified->setCheckable(true);
        actSortByModified->setChecked(sortType == QDir::Time);
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

void FileSystemView::scrollToPositionWithAnimation(const QPoint &pos)
{
    const int duration = 150;
    QPropertyAnimation *aniScrollH = new QPropertyAnimation(
        m_view->horizontalScrollBar(), "value");
    aniScrollH->setStartValue(m_view->horizontalScrollBar()->value());
    aniScrollH->setEndValue(pos.x());
    aniScrollH->setDuration(duration);
    QPropertyAnimation *aniScrollV = new QPropertyAnimation(
        m_view->verticalScrollBar(), "value");
    aniScrollV->setStartValue(m_view->verticalScrollBar()->value());
    aniScrollV->setEndValue(pos.y());
    aniScrollV->setDuration(duration);

    QParallelAnimationGroup *aniScroll = new QParallelAnimationGroup(this);
    aniScroll->addAnimation(aniScrollH);
    aniScroll->addAnimation(aniScrollV);

    aniScroll->start(QAbstractAnimation::DeleteWhenStopped);
}


#include "FileSystemView.moc"
