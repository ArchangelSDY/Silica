#include "playlist/FileSystemPlayListEntity.h"

#include <QCache>
#include <QCryptographicHash>
#include <QDateTime>
#include <QDirIterator>
#include <QEventLoop>
#include <QFileInfo>
#include <QMutex>

#include "image/ImageSourceManager.h"
#include "GlobalConfig.h"

static QCache<QString, QImage> g_coverCache(500);
static QCache<QString, bool> g_folderCover(500);
// QCache is not thread-safe for either read or write
static QMutex g_coverCacheLock;

static QString getCoverCacheKey(const QFileInfo &pathInfo)
{
    return QString("%1_%2")
        .arg(pathInfo.absoluteFilePath())
        .arg(QString::number(pathInfo.lastModified().toMSecsSinceEpoch()));
}

static QString computeThumbnailPath(const QFileInfo &pathInfo)
{
    QString pathHash = QCryptographicHash::hash(pathInfo.absoluteFilePath().toUtf8(), QCryptographicHash::Sha1).toHex();
    QString sub = pathHash.left(2);
    QString name = pathHash.mid(2);

    QStringList pathParts = QStringList() << GlobalConfig::instance()->fileSystemThumbnailPath() << sub << name << ".jpg";
    return pathParts.join("/");
}

static bool isThumbnailExpired(const QFileInfo &pathInfo, const QImage &image)
{
    QDateTime savedLastModified = QDateTime::fromString(image.text("lastModified"), Qt::DateFormat::ISODateWithMs);
    QDateTime curLastModified = pathInfo.lastModified();
    return savedLastModified != curLastModified;
}

FileSystemPlayListEntity::FileSystemPlayListEntity(FileSystemPlayListProvider *provider,
        const QFileInfo &fileInfo) :
    PlayListEntity(provider) ,
    m_fileInfo(fileInfo)
{
}

FileSystemPlayListEntity::~FileSystemPlayListEntity()
{
}

int FileSystemPlayListEntity::count() const
{
    Q_UNREACHABLE();
    return 0;
}

QString FileSystemPlayListEntity::name() const
{
    return m_fileInfo.fileName();
}

bool FileSystemPlayListEntity::supportsOption(PlayListEntityOption option) const
{
    return false;
}

// Cover image is loaded through following steps:
//
// 1. If already in cache, load from cache.
// 2.
//   a) If item is a directory, iterate its children to find a valid image
//      source.
//      i)  If valid image source found, use it as item cover.
//      ii) If valid image source not found, use default folder icon as cover.
//   b) If item is a regular file, check if it's a valid image source.
//      i)  If true, use it as item cover.
//      ii) If false, use default image icon as cover.
QImage FileSystemPlayListEntity::loadCoverImage()
{
    QString cacheKey = getCoverCacheKey(m_fileInfo);
    g_coverCacheLock.lock();
    QImage *cachedImage = g_coverCache[cacheKey];
    if (cachedImage) {
        QImage image(*cachedImage);
        g_coverCacheLock.unlock();
        return image;
    }

    bool *isDefaultFolderCover = g_folderCover[cacheKey];
    if (isDefaultFolderCover) {
        g_coverCacheLock.unlock();
        return QImage(":/res/folder.png");
    }
    g_coverCacheLock.unlock();

    QString thumbnailPath = computeThumbnailPath(m_fileInfo);
    if (QFileInfo::exists(thumbnailPath)) {
        QImage thumbnail(thumbnailPath);
        if (!isThumbnailExpired(m_fileInfo, thumbnail)) {
            return thumbnail;
        }
    }

    if (m_fileInfo.isDir()) {
        // For directory, try to use first image inside as cover
        QDirIterator dirIter(m_fileInfo.absoluteFilePath(),
                             QDir::Files);
        bool found = false;
        while (dirIter.hasNext()) {
            QString path = dirIter.next();
            if (!ImageSourceManager::instance()->isValidNameSuffix(dirIter.fileInfo().suffix())) {
                continue;
            }
            QSharedPointer<ImageSource> src(ImageSourceManager::instance()->createSingle(path));
            if (src) {
                Image image(src);
                QSharedPointer<QImage> thumbnail = image.loadThumbnailSync();
                if (thumbnail) {
                    g_coverCacheLock.lock();
                    g_coverCache.insert(cacheKey, new QImage(*thumbnail));
                    g_coverCacheLock.unlock();
                    return *thumbnail;
                }

            }
        }

        // No suitable image found.
        g_coverCacheLock.lock();
        g_folderCover.insert(cacheKey, new bool(true));
        g_coverCacheLock.unlock();
        return QImage(":/res/folder.png");
    } else {
        // Individual file
        QString path = m_fileInfo.absoluteFilePath();
        QSharedPointer<ImageSource> src(ImageSourceManager::instance()->createSingle(path));
        if (src) {
            // Load thumbnail for image file
            Image image(src);
            QSharedPointer<QImage> thumbnail = image.loadThumbnailSync();

            if (thumbnail) {
                g_coverCacheLock.lock();
                g_coverCache.insert(cacheKey, new QImage(*thumbnail));
                g_coverCacheLock.unlock();
                return *thumbnail;
            }
        }

        // Not valid image
        return QImage(":/res/image.png");
    }
}

QList<QUrl> FileSystemPlayListEntity::loadImageUrls()
{
    if (m_fileInfo.isDir()) {
        Q_UNREACHABLE();
        return {};
    }

    auto imageSources = ImageSourceManager::instance()->createMultiple(m_fileInfo.absoluteFilePath());
    if (imageSources.count() == 1) {
        qDeleteAll(imageSources);
        // Load all siblings if we only get one
        QFileInfoList entries = m_fileInfo.dir().entryInfoList(
            QDir::Files | QDir::NoDotAndDotDot,
            QDir::Name);
        QList<QUrl> imageUrls;
        for (const QFileInfo &info : entries) {
            if (ImageSourceManager::instance()->isValidNameSuffix(info.suffix())) {
                imageUrls << QUrl::fromLocalFile(info.absoluteFilePath());
            }
        }
        return imageUrls;
    } else {
        qDeleteAll(imageSources);
        // Do not use expanded urls. We should always pass around original url to batch create `ImageSource` efficiently
        return { QUrl::fromLocalFile(m_fileInfo.absoluteFilePath()) };
    }
}

void FileSystemPlayListEntity::setName(const QString &name)
{
    Q_UNREACHABLE();
}

void FileSystemPlayListEntity::setCoverImagePath(const QString &path)
{
    Q_UNREACHABLE();
}

void FileSystemPlayListEntity::addImageUrls(const QList<QUrl> &imageUrls)
{
    Q_UNREACHABLE();
}

void FileSystemPlayListEntity::removeImageUrls(const QList<QUrl> &imageUrls)
{
    Q_UNREACHABLE();
}

QFileInfo FileSystemPlayListEntity::fileInfo() const
{
    return m_fileInfo;
}
