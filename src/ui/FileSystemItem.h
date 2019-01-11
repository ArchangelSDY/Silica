#ifndef FILESYSTEMITEM_H
#define FILESYSTEMITEM_H

#include <QFileInfo>

#include "ui/GalleryItem.h"

class QThreadPool;
class Image;
class ImageSource;

class FileSystemItem : public GalleryItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
    friend class LoadRunnable;
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
    void markIsDefaultFolderCover(bool isDefault);
    void gotThumbnail(QSharedPointer<QImage> image);
    void loadCover(QSharedPointer<ImageSource> imageSource);
    void coverThumbnailLoaded(QSharedPointer<QImage> thumbnail);
    void coverThumbnailLoadFailed();

private:
    static QThreadPool *threadPool();
    static QThreadPool *s_threadPool;

    QString coverCacheKey() const;

    QFileInfo m_pathInfo;
    QScopedPointer<Image, QScopedPointerDeleteLater> m_coverImage;
    bool m_useDefaultFolderCover;
};

#endif // FILESYSTEMITEM_H
