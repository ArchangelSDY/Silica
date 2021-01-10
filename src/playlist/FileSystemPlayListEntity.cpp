#include "playlist/FileSystemPlayListEntity.h"

#include <QCache>
#include <QCryptographicHash>
#include <QDateTime>
#include <QDirIterator>
#include <QEventLoop>
#include <QFileInfo>
#include <QReadWriteLock>

#include "image/Image.h"
#include "image/ImageSourceManager.h"
#include "GlobalConfig.h"
#include "../PlayList.h"


static QCache<QString, QImage> g_coverCache(500);
static QCache<QString, bool> g_isDefaultFolderCover(500);
static QReadWriteLock g_coverCacheLock;

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
    g_coverCacheLock.lockForRead();
    QImage *cachedImage = g_coverCache[cacheKey];
    if (cachedImage) {
        QImage image(*cachedImage);
        g_coverCacheLock.unlock();
        return image;
    }

    bool *isDefaultFolderCover = g_isDefaultFolderCover[cacheKey];
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
                             ImageSourceManager::instance()->nameFilters(),
                             QDir::Files);
        bool found = false;
        while (dirIter.hasNext()) {
            QString path = dirIter.next();
            QSharedPointer<ImageSource> src(ImageSourceManager::instance()->createSingle(path));
            if (src) {
                Image image(src);
                QSharedPointer<QImage> thumbnail = image.loadThumbnailSync();
                if (thumbnail) {
                    g_coverCacheLock.lockForWrite();
                    g_coverCache.insert(cacheKey, new QImage(*thumbnail));
                    g_coverCacheLock.unlock();
                    return *thumbnail;
                }

            }
        }

        // No suitable image found.
        g_coverCacheLock.lockForWrite();
        g_isDefaultFolderCover.insert(cacheKey, new bool(true));
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
                g_coverCacheLock.lockForWrite();
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

    PlayList pl;
    pl.addMultiplePath(m_fileInfo.absoluteFilePath());

    if (pl.count() == 1) {
        // Add siblings too
        QDir curDir = m_fileInfo.dir();
        QFileInfoList entries = curDir.entryInfoList(
            ImageSourceManager::instance()->nameFilters(),
            QDir::Files | QDir::NoDotAndDotDot,
            QDir::Name);
        for (const QFileInfo &info : entries) {
            // Avoid duplicate
            if (info.absoluteFilePath() != m_fileInfo.absoluteFilePath()) {
                pl.addSinglePath(info.absoluteFilePath());
            }
        }
    }

    return pl.toImageUrls();
}

void FileSystemPlayListEntity::setName(const QString &name)
{
    // TODO
}

void FileSystemPlayListEntity::setCoverImagePath(const QString &path)
{
    // TODO
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
