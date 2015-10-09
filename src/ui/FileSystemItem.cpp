#include <QCache>
#include <QDirIterator>
#include <QFileInfo>
#include <QThreadPool>

#include "image/Image.h"
#include "image/ImageSourceManager.h"
#include "ui/FileSystemItem.h"
#include "ui/GalleryView.h"

// Item cover is loaded through following steps:
//
// 1. If already in cache, load from cache.
// 2. Spawn a background worker thread.
//   a) If item is a directory, iterate its children to find a valid image
//      source.
//      i)  If valid image source found, use it as item cover.
//      ii) If valid image source not found, use default folder icon as cover.
//   b) If item is a regular file, check if it's a valid image source.
//      i)  If true, use it as item cover.
//      ii) If false, use default image icon as cover.
// Note that image source is always loaded asyncly.
//
// When a directory item uses its child image source as cover, a folder icon is
// shown at top-right corner. However, if a directory item has no valid child
// image source and uses default folder icon as its cover, the corner icon is no
// longer needed. This logic is controlled by `m_useDefaultFolderCover` and
// `markIsDefaultFolderCover()`.

static QCache<QString, QImage> g_coverCache(500);
static QCache<QString, bool> g_isDefaultFolderCover(500);

class LoadRunnable : public QObject, public QRunnable
{
    Q_OBJECT
public:
    LoadRunnable(const QFileInfo &pathInfo) : m_pathInfo(pathInfo) {}

    virtual void run()
    {
        if (m_pathInfo.isDir()) {
            // For directory, try to use first image inside as cover
            QDirIterator dirIter(m_pathInfo.absoluteFilePath(),
                                 ImageSourceManager::instance()->nameFilters(),
                                 QDir::Files);
            bool found = false;
            while (dirIter.hasNext()) {
                QString path = dirIter.next();
                ImageSource *src = ImageSourceManager::instance()->createSingle(
                    path);
                if (src) {
                    emit markIsDefaultFolderCover(false);
                    emit loadCover(path);
                    found = true;
                    delete src;
                    break;
                }
            }

            if (!found) {
                // No suitable image found.
                emit markIsDefaultFolderCover(true);
                emit gotThumbnail(":/res/folder.png");
            }
        } else {
            // Individual file
            QString path = m_pathInfo.absoluteFilePath();
            ImageSource *src =
                ImageSourceManager::instance()->createSingle(path);
            if (src) {
                // Load thumbnail for image file
                emit loadCover(path);
                delete src;
            } else {
                // Not valid image
                emit gotThumbnail(":/res/image.png");
            }
        }
    }

signals:
    void markIsDefaultFolderCover(bool isDefault);
    void gotThumbnail(QString path);
    void loadCover(QString path);

private:
    QFileInfo m_pathInfo;
};

QThreadPool *FileSystemItem::s_threadPool = 0;
QThreadPool *FileSystemItem::threadPool()
{
    if (!s_threadPool) {
        s_threadPool = new QThreadPool();
    }
    return s_threadPool;
}

FileSystemItem::FileSystemItem(const QString &path,
                               AbstractRendererFactory *rendererFactory,
                               QGraphicsItem *parent) :
    GalleryItem(rendererFactory, parent) ,
    m_pathInfo(QFileInfo(path)) ,
    m_coverImage(0) ,
    m_useDefaultFolderCover(true)
{
    setFlag(QGraphicsItem::ItemIsSelectable);
    setToolTip(m_pathInfo.fileName());
    createRenderer();
}

FileSystemItem::~FileSystemItem()
{
    if (m_coverImage) {
        m_coverImage->deleteLater();
    }
}

QString FileSystemItem::name() const
{
    return m_pathInfo.fileName();
}

QRectF FileSystemItem::boundingRect() const
{
    return m_renderer->boundingRect();
}

QString FileSystemItem::path() const
{
    return m_pathInfo.absoluteFilePath();
}

QFileInfo FileSystemItem::fileInfo() const
{
    return m_pathInfo;
}

void FileSystemItem::load()
{
    if (!m_rendererFactory) {
        return;
    }

    QImage *cachedCover = g_coverCache[coverCacheKey()];
    if (cachedCover) {
        bool *isDefaultFolderCover = g_isDefaultFolderCover[coverCacheKey()];
        markIsDefaultFolderCover(
            isDefaultFolderCover ? *isDefaultFolderCover : true);
        setThumbnail(new QImage(*cachedCover));
    } else {
        LoadRunnable *r = new LoadRunnable(m_pathInfo);
        connect(r, SIGNAL(markIsDefaultFolderCover(bool)),
                this, SLOT(markIsDefaultFolderCover(bool)));
        connect(r, SIGNAL(gotThumbnail(QString)),
                this, SLOT(gotThumbnail(QString)));
        connect(r, SIGNAL(loadCover(QString)),
                this, SLOT(loadCover(QString)));
        threadPool()->start(r);
    }
}

void FileSystemItem::markIsDefaultFolderCover(bool isDefault)
{
    g_isDefaultFolderCover.insert(coverCacheKey(), new bool(isDefault));
    m_useDefaultFolderCover = isDefault;
    // Re-create render here to refresh "m_useDefaultFolderCover"
    createRenderer();
}

void FileSystemItem::gotThumbnail(QString path)
{
    QImage *coverImage = new QImage(path);
    g_coverCache.insert(coverCacheKey(), coverImage);
    setThumbnail(new QImage(*coverImage));
}

void FileSystemItem::createRenderer()
{
    setRenderer(m_rendererFactory->createItemRendererForFileSystemView(
        m_pathInfo, m_useDefaultFolderCover));
}

void FileSystemItem::coverThumbnailLoaded()
{
    QImage thumbnail = m_coverImage->thumbnail();
    QImage *coverImage = 0;
    if (!thumbnail.isNull()) {
        coverImage = new QImage(m_coverImage->thumbnail());
    } else {
        coverImage = new QImage(":/res/image.png");
    }
    g_coverCache.insert(coverCacheKey(), coverImage);
    setThumbnail(new QImage(*coverImage));
    m_coverImage->deleteLater();
    m_coverImage = 0;
}

void FileSystemItem::coverThumbnailLoadFailed()
{
    QImage *coverImage = new QImage(":/res/image.png");
    g_coverCache.insert(coverCacheKey(), coverImage);
    setThumbnail(new QImage(*coverImage));
    m_coverImage->deleteLater();
    m_coverImage = 0;
}

void FileSystemItem::loadCover(QString path)
{
    ImageSource *src = ImageSourceManager::instance()->createSingle(path);
    m_coverImage = new Image(src);
    connect(m_coverImage, SIGNAL(thumbnailLoaded()),
            this, SLOT(coverThumbnailLoaded()));
    connect(m_coverImage, SIGNAL(thumbnailLoadFailed()),
            this, SLOT(coverThumbnailLoadFailed()));
    m_coverImage->loadThumbnail(boundingRect().size().toSize(), true);
}

void FileSystemItem::removeOnDisk()
{
    QFileInfo info(path());
    if (info.isDir()) {
        QDir dir(path());
        dir.removeRecursively();
    } else {
        QFile::remove(path());
    }
}

QString FileSystemItem::coverCacheKey() const
{
    return QString("%1_%2")
        .arg(m_pathInfo.absoluteFilePath())
        .arg(QString::number(m_pathInfo.lastModified().toMSecsSinceEpoch()));
}


#include "FileSystemItem.moc"
