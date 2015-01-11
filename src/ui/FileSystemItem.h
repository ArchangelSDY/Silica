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
    void coverThumbnailLoaded();
    void coverThumbnailLoadFailed();

private:
    void loadCover(ImageSource *src);

    QString m_path;
    Image *m_coverImage;
};

#endif // FILESYSTEMITEM_H
