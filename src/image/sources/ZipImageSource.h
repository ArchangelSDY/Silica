#ifndef ZIPIMAGESOURCE_H
#define ZIPIMAGESOURCE_H

#include "ImageSource.h"

class Qt7zPackage;

class ZipImageSource : public ImageSource
{
public:
    ZipImageSource(QString zipPath, QString imageName);

    virtual bool open();

    virtual bool copy(const QString &destPath);
private:
    QString m_zipPath;
};

#endif // ZIPIMAGESOURCE_H
