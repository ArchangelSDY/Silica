#include <QCache>
#include <QDirIterator>
#include <QFileInfo>
#include <QThreadPool>

#include "image/Image.h"
#include "image/ImageSourceManager.h"
#include "ui/FileSystemItem.h"
#include "ui/GalleryView.h"

static QCache<QString, QImage> g_coverCache(500);

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

        emit done();
    }

signals:
    void markIsDefaultFolderCover(bool isDefault);
    void gotThumbnail(QString path);
    void loadCover(QString path);
    void done();

private:
    QFileInfo m_pathInfo;
};

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
        setThumbnail(new QImage(*cachedCover));
    } else {
        LoadRunnable *r = new LoadRunnable(m_pathInfo);
        connect(r, SIGNAL(markIsDefaultFolderCover(bool)),
                this, SLOT(markIsDefaultFolderCover(bool)));
        connect(r, SIGNAL(gotThumbnail(QString)),
                this, SLOT(gotThumbnail(QString)));
        connect(r, SIGNAL(loadCover(QString)),
                this, SLOT(loadCover(QString)));
        connect(r, SIGNAL(done()),
                this, SLOT(loaded()));
        QThreadPool::globalInstance()->start(r);
    }
}

void FileSystemItem::markIsDefaultFolderCover(bool isDefault)
{
    m_useDefaultFolderCover = isDefault;
}

void FileSystemItem::gotThumbnail(QString path)
{
    QImage *coverImage = new QImage(path);
    g_coverCache.insert(coverCacheKey(), coverImage);
    setThumbnail(new QImage(*coverImage));
}

void FileSystemItem::loaded()
{
    // Re-create render here to refresh "m_useDefaultFolderCover"
    createRenderer();
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
    m_coverImage->loadThumbnail(true);
}

void FileSystemItem::removeOnDisk()
{
    QFile::remove(path());
}

QString FileSystemItem::coverCacheKey() const
{
    return QString("%1_%2")
        .arg(m_pathInfo.absoluteFilePath())
        .arg(QString::number(m_pathInfo.lastModified().toMSecsSinceEpoch()));
}


#include "FileSystemItem.moc"
