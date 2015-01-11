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
        delete m_coverImage;
    }
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

    if (m_pathInfo.isDir()) {
        // For directory, try to use first image inside as cover
        QDirIterator dirIter(m_pathInfo.absoluteFilePath(),
                             ImageSourceManager::instance()->nameFilters(),
                             QDir::Files);
        bool found = false;
        while (dirIter.hasNext()) {
            ImageSource *src = ImageSourceManager::instance()->createSingle(
                dirIter.next());
            if (src) {
                m_useDefaultFolderCover = false;
                loadCover(src);
                found = true;
                break;
            }
        }

        if (!found) {
            // No suitable image found.
            setThumbnail(new QImage(":/res/folder.png"));
        }
    } else {
        // Individual file
        ImageSource *src = ImageSourceManager::instance()->createSingle(
            m_pathInfo.absoluteFilePath());
        if (src) {
            // Load thumbnail for image file
            loadCover(src);
        } else {
            // Not valid image
            setThumbnail(new QImage(":/res/image.png"));
        }
    }

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

