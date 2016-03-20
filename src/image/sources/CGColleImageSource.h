#ifndef CGCOLLEIMAGESOURCE_H
#define CGCOLLEIMAGESOURCE_H

#include "image/ImageSource.h"

class ImageSourceFactory;

class CGColleImageSource : public ImageSource
{
public:
    CGColleImageSource(ImageSourceFactory *factory,
                       QString CGCollePath, QString imageName);

    virtual bool open();
    virtual bool exists();

    virtual bool copy(const QString &destPath);
private:
    QString m_packagePath;
};

#endif // CGCOLLEIMAGESOURCE_H