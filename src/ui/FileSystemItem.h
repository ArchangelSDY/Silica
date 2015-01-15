#ifndef FILESYSTEMITEM_H
#define FILESYSTEMITEM_H

#include "ui/GalleryItem.h"

class QFileInfo;
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

    QRectF boundingRect() const;
    QString path() const;
    QFileInfo fileInfo() const;

    void load();
    virtual void createRenderer();

private slots:
    void gotThumbnail(QString path);
    void loadCover(QString path);
    void loaded();
    void coverThumbnailLoaded();
    void coverThumbnailLoadFailed();

private:
    QFileInfo m_pathInfo;
    Image *m_coverImage;
    bool m_useDefaultFolderCover;
};

#endif // FILESYSTEMITEM_H
