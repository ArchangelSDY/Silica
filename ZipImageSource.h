#ifndef ZIPIMAGESOURCE_H
#define ZIPIMAGESOURCE_H

#include "ImageSource.h"

class ZipImageSource : public ImageSource
{
public:
    ZipImageSource(QString zipPath, QString imageName);

    virtual bool open();

private:
    QString m_zipPath;
    QString m_imageName;
};

#endif // ZIPIMAGESOURCE_H
