#ifndef CGCOLLEIMAGESOURCE_H
#define CGCOLLEIMAGESOURCE_H

#include "image/sources/CGColleReader.h"
#include "image/ImageSource.h"

class ImageSourceFactory;

class CGColleImageSource : public ImageSource
{
public:
    CGColleImageSource(ImageSourceFactory *factory,
                       QString CGCollePath, QString imageName,
                       CGColleReader::ImageReader *reader);

    virtual bool open();
    virtual bool exists();

    virtual bool copy(const QString &destPath);
private:
    QString m_packagePath;
    QScopedPointer<CGColleReader::ImageReader> m_reader;
};

#endif // CGCOLLEIMAGESOURCE_H