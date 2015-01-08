#include <QDir>
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
    setRenderer(m_rendererFactory->createItemRendererForFileSystemView(
        info.fileName()));
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

    // TODO(sdy): do at background
    QFileInfo finfo(m_path);
    if (finfo.isDir()) {
        // For directory, try to use first image inside as cover

        QDir dir(finfo.absoluteFilePath());
        QFileInfoList inDirImageInfos = dir.entryInfoList(
            ImageSourceManager::instance()->nameFilters(),
            QDir::Files);

        // Find an image to show as cover
        bool found = false;
        for (int i = 0; i < inDirImageInfos.count(); ++i) {
            ImageSource *src = ImageSourceManager::instance()->createSingle(
                inDirImageInfos[i].absoluteFilePath());
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

void FileSystemItem::setRendererFactory(AbstractRendererFactory *factory)
{
    m_rendererFactory = factory;
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

