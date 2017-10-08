#pragma once

#include <QEventLoop>

#include "Image.h"

class ImageThumbnailBlockLoader : public QObject
{
    Q_OBJECT
public:
    ImageThumbnailBlockLoader(ImagePtr image);

    void loadAndWait();
    QSharedPointer<QImage> thumbnail() const;

private slots:
    void thumbnailLoaded(QSharedPointer<QImage> thumbnail);

private:
    QEventLoop m_loop;
    ImagePtr m_image;
    QSharedPointer<QImage> m_thumbnail;
};
