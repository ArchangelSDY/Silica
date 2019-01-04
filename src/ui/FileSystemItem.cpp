#include <QCache>
#include <QCryptographicHash>
#include <QDirIterator>
#include <QFileInfo>
#include <QThreadPool>

#include "image/Image.h"
#include "image/ImageSourceManager.h"
#include "ui/FileSystemItem.h"
#include "ui/GalleryView.h"
#include "GlobalConfig.h"

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

static QString computeThumbnailPath(const QFileInfo &pathInfo)
{
    QString pathHash = QCryptographicHash::hash(pathInfo.absoluteFilePath().toUtf8(), QCryptographicHash::Sha1).toHex();
    QString sub = pathHash.left(2);
    QString name = pathHash.mid(2);

    QStringList pathParts = QStringList() << GlobalConfig::instance()->fileSystemThumbnailPath() << sub << name << ".jpg";
    return pathParts.join("/");
}

class LoadRunnable : public QObject, public QRunnable
{
    Q_OBJECT
public:
    LoadRunnable(const QFileInfo &pathInfo) : m_pathInfo(pathInfo) {}

    virtual void run()
    {
        QString thumbnailPath = computeThumbnailPath(m_pathInfo);
        if (QFileInfo::exists(thumbnailPath)) {
            QImage thumbnail(thumbnailPath);
            if (!isThumbnailExpired(thumbnail)) {
                emit gotThumbnail(thumbnail);
                return;
            }
        }

        if (m_pathInfo.isDir()) {
            // For directory, try to use first image inside as cover
            QDirIterator dirIter(m_pathInfo.absoluteFilePath(),
                                 ImageSourceManager::instance()->nameFilters(),
                                 QDir::Files);
            bool found = false;
            while (dirIter.hasNext()) {
                QString path = dirIter.next();
                QSharedPointer<ImageSource> src(ImageSourceManager::instance()->createSingle(path));
                if (src) {
                    emit markIsDefaultFolderCover(false);
                    emit loadCover(src);
                    found = true;
                    break;
                }
            }

            if (!found) {
                // No suitable image found.
                emit markIsDefaultFolderCover(true);
                emit gotThumbnail(QImage(":/res/folder.png"));
            }
        } else {
            // Individual file
            QString path = m_pathInfo.absoluteFilePath();
            QSharedPointer<ImageSource> src(ImageSourceManager::instance()->createSingle(path));
            if (src) {
                // Load thumbnail for image file
                emit loadCover(src);
            } else {
                // Not valid image
                emit gotThumbnail(QImage(":/res/image.png"));
            }
        }
    }

signals:
    void gotThumbnail(const QImage &image);
    void markIsDefaultFolderCover(bool isDefault);
    void loadCover(QSharedPointer<ImageSource> imageSource);

private:

    bool isThumbnailExpired(const QImage &image) const
    {
        QDateTime savedLastModified = QDateTime::fromString(image.text("lastModified"), Qt::DateFormat::ISODateWithMs);
        QDateTime curLastModified = m_pathInfo.lastModified();

        return savedLastModified != curLastModified;
    }

    QFileInfo m_pathInfo;
};


class SaveRunnable : public QRunnable
{
public:
    SaveRunnable(const QImage &image, const QFileInfo &pathInfo) :
        m_image(image) ,
        m_pathInfo(pathInfo)
    {
    }

    virtual void run()
    {
        QString thumbnailPath = computeThumbnailPath(m_pathInfo);

        // Create parent directory
        QFileInfo thumbnailPathInfo(thumbnailPath);
        QString dirPath = thumbnailPathInfo.absolutePath();
        QDir dir;
        dir.mkpath(dirPath);

        m_image.setText("lastModified", m_pathInfo.lastModified().toString(Qt::DateFormat::ISODateWithMs));
        m_image.save(thumbnailPath);
    }

private:
    QImage m_image;
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
    m_coverImage(nullptr) ,
    m_useDefaultFolderCover(true)
{
    setFlag(QGraphicsItem::ItemIsSelectable);
    setToolTip(m_pathInfo.fileName());
    createRenderer();
}

FileSystemItem::~FileSystemItem()
{
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

    QImage *cover = g_coverCache[coverCacheKey()];
    if (cover) {
        markIsDefaultFolderCover(false);
        gotThumbnail(*cover);
        return;
    }

    bool *isDefaultFolderCover = g_isDefaultFolderCover[coverCacheKey()];
    if (isDefaultFolderCover) {
        markIsDefaultFolderCover(true);
        gotThumbnail(QImage(":/res/folder.png"));
        return;
    }

    LoadRunnable *r = new LoadRunnable(m_pathInfo);
    connect(r, SIGNAL(markIsDefaultFolderCover(bool)),
            this, SLOT(markIsDefaultFolderCover(bool)));
    connect(r, SIGNAL(gotThumbnail(const QImage &)),
            this, SLOT(gotThumbnail(const QImage &)));
    connect(r, SIGNAL(loadCover(QSharedPointer<ImageSource>)),
            this, SLOT(loadCover(QSharedPointer<ImageSource>)));
    threadPool()->start(r);
}

void FileSystemItem::markIsDefaultFolderCover(bool isDefault)
{
    if (isDefault) {
        g_isDefaultFolderCover.insert(coverCacheKey(), new bool(true));
    }

    m_useDefaultFolderCover = isDefault;
    // Re-create render here to refresh "m_useDefaultFolderCover"
    createRenderer();
}

void FileSystemItem::gotThumbnail(const QImage &image)
{
    setThumbnail(new QImage(image));
}

void FileSystemItem::createRenderer()
{
    setRenderer(m_rendererFactory->createItemRendererForFileSystemView(
        m_pathInfo, m_useDefaultFolderCover));
}

void FileSystemItem::coverThumbnailLoaded(QSharedPointer<QImage> thumbnail)
{
    QImage *coverImage = 0;
    if (!thumbnail->isNull()) {
        coverImage = new QImage(*thumbnail);

        threadPool()->start(new SaveRunnable(*coverImage, m_pathInfo));
    } else {
        coverImage = new QImage(":/res/image.png");
    }

    g_coverCache.insert(coverCacheKey(), coverImage);

    setThumbnail(new QImage(*coverImage));
    m_coverImage.reset();
}

void FileSystemItem::coverThumbnailLoadFailed()
{
    QImage *coverImage = new QImage(":/res/image.png");
    g_coverCache.insert(coverCacheKey(), coverImage);
    setThumbnail(new QImage(*coverImage));
    m_coverImage.reset();
}

void FileSystemItem::loadCover(QSharedPointer<ImageSource> imageSource)
{
    m_coverImage.reset(new Image(imageSource));
    connect(m_coverImage.data(), &Image::thumbnailLoaded,
            this, &FileSystemItem::coverThumbnailLoaded);
    connect(m_coverImage.data(), &Image::thumbnailLoadFailed,
            this, &FileSystemItem::coverThumbnailLoadFailed);
    m_coverImage->loadThumbnail();
}

void FileSystemItem::refresh()
{
    QFile::remove(computeThumbnailPath(path()));
    load();
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
