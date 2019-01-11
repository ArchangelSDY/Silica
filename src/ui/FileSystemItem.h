#ifndef FILESYSTEMITEM_H
#define FILESYSTEMITEM_H

#include <QFileInfo>
#include <QFutureWatcher>

#include "ui/GalleryItem.h"

class QThreadPool;
class Image;
class ImageSource;

class FileSystemItem : public GalleryItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    explicit FileSystemItem(const QString &path,
                            AbstractRendererFactory *rendererFactory,
                            QGraphicsItem *parent = 0);
    virtual ~FileSystemItem();

    QString name() const;
    QRectF boundingRect() const;
    QString path() const;
    QFileInfo fileInfo() const;

    virtual void load() override;
    virtual void createRenderer();
    void refresh();
    void removeOnDisk();

private slots:
    void loaded();
    void coverThumbnailLoaded(QSharedPointer<QImage> thumbnail);
    void coverThumbnailLoadFailed();

private:
    static QThreadPool *threadPool();
    static QThreadPool *s_threadPool;

    void markIsDefaultFolderCover(bool isDefault);

    struct LoadResult
    {
        QSharedPointer<ImageSource> coverImageSource;
        QSharedPointer<QImage> image;
        bool useDefaultFolderCover = false;
    };

    QString coverCacheKey() const;

    QFileInfo m_pathInfo;
    QScopedPointer<Image, QScopedPointerDeleteLater> m_coverImage;
    bool m_useDefaultFolderCover;

    QFutureWatcher<LoadResult> m_loader;
};

#endif // FILESYSTEMITEM_H
