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
    m_coverImage(0)
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
                loadCover(src);
                found = true;
                break;
            }
        }

        if (!found) {
            // No suitable image found.
            //
            // Since it's a folder, we set a dummy thumbnail and leave render
            // factory to create background icon. Null image cannot be used here
            // because it will not trigger a paint.
            setThumbnail(new QImage(1, 1, QImage::Format_ARGB32));
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
}

void FileSystemItem::createRenderer()
{
    setRenderer(
        m_rendererFactory->createItemRendererForFileSystemView(m_pathInfo));
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

