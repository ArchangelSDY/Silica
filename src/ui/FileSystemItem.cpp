#include <QCache>
#include <QCryptographicHash>
#include <QDirIterator>
#include <QFileInfo>
#include <QThreadPool>
#include <QtConcurrent>

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

static bool isThumbnailExpired(const QFileInfo &pathInfo, QSharedPointer<QImage> image)
{
    QDateTime savedLastModified = QDateTime::fromString(image->text("lastModified"), Qt::DateFormat::ISODateWithMs);
    QDateTime curLastModified = pathInfo.lastModified();

    return savedLastModified != curLastModified;
}

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
    connect(&m_loader, &QFutureWatcher<LoadResult>::finished, this, &FileSystemItem::loaded);
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

    QImage *image = g_coverCache[coverCacheKey()];
    if (image) {
        markIsDefaultFolderCover(false);
        setThumbnail(QSharedPointer<QImage>::create(*image));
        return;
    }

    bool *isDefaultFolderCover = g_isDefaultFolderCover[coverCacheKey()];
    if (isDefaultFolderCover) {
        markIsDefaultFolderCover(true);
        setThumbnail(QSharedPointer<QImage>::create(":/res/folder.png"));
        return;
    }

    m_loader.setFuture(QtConcurrent::run(threadPool(), [pathInfo = m_pathInfo]() -> LoadResult {
        FileSystemItem::LoadResult result;

        QString thumbnailPath = computeThumbnailPath(pathInfo);
        if (QFileInfo::exists(thumbnailPath)) {
            QSharedPointer<QImage> thumbnail(new QImage(thumbnailPath));
            if (!isThumbnailExpired(pathInfo, thumbnail)) {
                result.image = thumbnail;
                return result;
            }
        }

        if (pathInfo.isDir()) {
            // For directory, try to use first image inside as cover
            QSet<QString> validNameSuffixes = QSet<QString>::fromList(ImageSourceManager::instance()->nameSuffixes());
            QDirIterator dirIter(pathInfo.absoluteFilePath(),
                                 QDir::Files);
            bool found = false;
            while (dirIter.hasNext()) {
                QString path = dirIter.next();
                if (!validNameSuffixes.contains(dirIter.fileInfo().suffix())) {
                    continue;
                }
                QSharedPointer<ImageSource> src(ImageSourceManager::instance()->createSingle(path));
                if (src) {
                    result.useDefaultFolderCover = false;
                    result.coverImageSource = src;
                    return result;
                }
            }

            // No suitable image found.
            result.useDefaultFolderCover = true;
            result.image = QSharedPointer<QImage>::create(":/res/folder.png");
        } else {
            // Individual file
            QString path = pathInfo.absoluteFilePath();
            QSharedPointer<ImageSource> src(ImageSourceManager::instance()->createSingle(path));
            if (src) {
                // Load thumbnail for image file
                result.coverImageSource = src;
            } else {
                // Not valid image
                result.image = QSharedPointer<QImage>::create(":/res/image.png");
            }
        }

        return result;
    }));
}

void FileSystemItem::loaded()
{
    auto result = m_loader.result();
    markIsDefaultFolderCover(result.useDefaultFolderCover);

    if (result.image) {
        // We have a loaded image
        setThumbnail(result.image);
    } else {
        // We do not have a loaded image but have an image source
        Q_ASSERT(result.coverImageSource);
        m_coverImage.reset(new Image(result.coverImageSource));
        connect(m_coverImage.data(), &Image::thumbnailLoaded,
                this, &FileSystemItem::coverThumbnailLoaded);
        connect(m_coverImage.data(), &Image::thumbnailLoadFailed,
                this, &FileSystemItem::coverThumbnailLoadFailed);
        m_coverImage->loadThumbnail();
    }
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

void FileSystemItem::createRenderer()
{
    setRenderer(m_rendererFactory->createItemRendererForFileSystemView(
        m_pathInfo, m_useDefaultFolderCover));
}

void FileSystemItem::coverThumbnailLoaded(QSharedPointer<QImage> thumbnail)
{
    QSharedPointer<QImage> coverImage;
    if (!thumbnail->isNull()) {
        coverImage.reset(new QImage(*thumbnail));

        QtConcurrent::run(threadPool(), [pathInfo = m_pathInfo, coverImage]() {
            QString thumbnailPath = computeThumbnailPath(pathInfo);

            // Create parent directory
            QFileInfo thumbnailPathInfo(thumbnailPath);
            QString dirPath = thumbnailPathInfo.absolutePath();
            QDir dir;
            dir.mkpath(dirPath);

            coverImage->setText("lastModified", pathInfo.lastModified().toString(Qt::DateFormat::ISODateWithMs));
            coverImage->save(thumbnailPath);
        });
    } else {
        coverImage.reset(new QImage(":/res/image.png"));
    }

    g_coverCache.insert(coverCacheKey(), new QImage(*coverImage));

    setThumbnail(coverImage);
    m_coverImage.reset();
}

void FileSystemItem::coverThumbnailLoadFailed()
{
    QImage *coverImage = new QImage(":/res/image.png");
    g_coverCache.insert(coverCacheKey(), coverImage);
    setThumbnail(QSharedPointer<QImage>::create(*coverImage));
    m_coverImage.reset();
}

void FileSystemItem::refresh()
{
    QFile::remove(computeThumbnailPath(path()));
    g_coverCache.remove(coverCacheKey());
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