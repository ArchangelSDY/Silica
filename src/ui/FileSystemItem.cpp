#include <QDirIterator>
#include <QFileInfo>

#include "image/Image.h"
#include "image/ImageSourceManager.h"
#include "ui/FileSystemItem.h"
#include "ui/GalleryView.h"

FileSystemItem::FileSystemItem(const QString &path,
                               AbstractRendererFactory *rendererFactory,
                               QGraphicsItem *parent) :
    GalleryItem(rendererFactory, parent) ,
    m_path(path) ,
    m_coverImage(0)
{
    setFlag(QGraphicsItem::ItemIsSelectable);

    QFileInfo info(m_path);
    setToolTip(info.fileName());
    createRenderer();
}

FileSystemItem::~FileSystemItem()
{
    if (m_coverImage) {
        delete m_coverImage;
    }
}

QRectF FileSystemItem::boundingRect() const
{
    return m_renderer->boundingRect();
}

QString FileSystemItem::path() const
{
    return m_path;
}

QFileInfo FileSystemItem::fileInfo() const
{
    return QFileInfo(m_path);
}

void FileSystemItem::load()
{
    if (!m_rendererFactory) {
        return;
    }

    if (m_thumbnail) {
        delete m_thumbnail;
    }

    QFileInfo finfo(m_path);
    if (finfo.isDir()) {
        // For directory, try to use first image inside as cover
        QDirIterator dirIter(finfo.absoluteFilePath(),
                             ImageSourceManager::instance()->nameFilters(),
                             QDir::Files);
        bool found = false;
        while (dirIter.hasNext()) {
            ImageSource *src = ImageSourceManager::instance()->createSingle(
                dirIter.next());
            if (src) {
                loadCover(src);
                found = true;
                break;
            }
        }

        if (!found) {
            // No suitable image found
            setThumbnail(new QImage(":/res/folder.png"));
        }
    } else {
        // Individual file
        ImageSource *src = ImageSourceManager::instance()->createSingle(
            finfo.absoluteFilePath());
        if (src) {
            // Load thumbnail for image file
            loadCover(src);
        } else {
            // Not valid image
            setThumbnail(new QImage(":/res/image.png"));
        }
    }
}

void FileSystemItem::createRenderer()
{
    setRenderer(m_rendererFactory->createItemRendererForFileSystemView(m_path));
}

void FileSystemItem::coverThumbnailLoaded()
{
    QImage thumbnail = m_coverImage->thumbnail();
    if (!thumbnail.isNull()) {
        setThumbnail(new QImage(m_coverImage->thumbnail()));
    } else {
        setThumbnail(new QImage(":/res/image.png"));
    }
    delete m_coverImage;
    m_coverImage = 0;
}

void FileSystemItem::coverThumbnailLoadFailed()
{
    setThumbnail(new QImage(":/res/image.png"));
    delete m_coverImage;
    m_coverImage = 0;
}

void FileSystemItem::loadCover(ImageSource *src)
{
    m_coverImage = new Image(src);
    connect(m_coverImage, SIGNAL(thumbnailLoaded()),
            this, SLOT(coverThumbnailLoaded()));
    connect(m_coverImage, SIGNAL(thumbnailLoadFailed()),
            this, SLOT(coverThumbnailLoadFailed()));
    m_coverImage->loadThumbnail(true);
}

