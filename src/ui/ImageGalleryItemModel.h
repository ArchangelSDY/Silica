#ifndef IMAGEGALLERYITEMMODEL_H
#define IMAGEGALLERYITEMMODEL_H

#include "AbstractGalleryItemModel.h"
#include "Image.h"

class ImageGalleryItemModel : public AbstractGalleryItemModel
{
public:
    explicit ImageGalleryItemModel(Image *image, QObject *parent = 0);

    void loadThumbnail();
    QImage thumbnail() const;

private:
    Image *m_image;
};

#endif // IMAGEGALLERYITEMMODEL_H
