#ifndef ZIPIMAGESOURCE_H
#define ZIPIMAGESOURCE_H

#include "image/ImageSource.h"

class ImageSourceFactory;

class ZipImageSource : public ImageSource
{
public:
    ZipImageSource(ImageSourceFactory *factory,
                   QString zipPath, QString imageName);

    virtual bool open();

    virtual bool copy(const QString &destPath);
private:
    QString m_zipPath;
};

#endif // ZIPIMAGESOURCE_H
