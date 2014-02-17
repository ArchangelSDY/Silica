#ifndef ABSTRACTGALLERYITEMMODEL_H
#define ABSTRACTGALLERYITEMMODEL_H

#include <QObject>
#include <QImage>

class AbstractGalleryItemModel : public QObject
{
    Q_OBJECT
public:
    explicit AbstractGalleryItemModel(QObject *parent = 0) :
        QObject(parent) {}

    virtual void loadThumbnail() = 0;
    virtual QImage thumbnail() const = 0;

signals:
    void thumbnailLoaded();
};

#endif // ABSTRACTGALLERYITEMMODEL_H
