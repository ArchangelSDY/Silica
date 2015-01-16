#include <QDirIterator>
#include <QFileInfo>
#include <QThreadPool>

#include "image/Image.h"
#include "image/ImageSourceManager.h"
#include "ui/FileSystemItem.h"
#include "ui/GalleryView.h"

class LoadRunnable : public QObject, public QRunnable
{
    Q_OBJECT
public:
    LoadRunnable(FileSystemItem *item) : m_item(item) {}

    virtual void run()
    {
        if (m_item->m_pathInfo.isDir()) {
            // For directory, try to use first image inside as cover
            QDirIterator dirIter(m_item->m_pathInfo.absoluteFilePath(),
                                 ImageSourceManager::instance()->nameFilters(),
                                 QDir::Files);
            bool found = false;
            while (dirIter.hasNext()) {
                QString path = dirIter.next();
                ImageSource *src = ImageSourceManager::instance()->createSingle(
                    path);
                if (src) {
                    m_item->m_useDefaultFolderCover = false;
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
            QString path = m_item->m_pathInfo.absoluteFilePath();
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
    void gotThumbnail(QString path);
    void loadCover(QString path);
    void done();

private:
    FileSystemItem *m_item;
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

    if (m_thumbnail) {
        delete m_thumbnail;
    }

    LoadRunnable *r = new LoadRunnable(this);
    connect(r, SIGNAL(gotThumbnail(QString)),
            this, SLOT(gotThumbnail(QString)));
    connect(r, SIGNAL(loadCover(QString)),
            this, SLOT(loadCover(QString)));
    connect(r, SIGNAL(done()),
            this, SLOT(loaded()));
    QThreadPool::globalInstance()->start(r);
}

void FileSystemItem::gotThumbnail(QString path)
{
    setThumbnail(new QImage(path));
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
    if (!thumbnail.isNull()) {
        setThumbnail(new QImage(m_coverImage->thumbnail()));
    } else {
        setThumbnail(new QImage(":/res/image.png"));
    }
    m_coverImage->deleteLater();
    m_coverImage = 0;
}

void FileSystemItem::coverThumbnailLoadFailed()
{
    setThumbnail(new QImage(":/res/image.png"));
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


#include "FileSystemItem.moc"
