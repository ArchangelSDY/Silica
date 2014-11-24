#ifndef SEVENZIMAGESOURCE_H
#define SEVENZIMAGESOURCE_H

#include <QBuffer>

#include "ImageSource.h"

class Qt7zPackage;

class SevenzImageSource : public ImageSource
{
public:
    SevenzImageSource(ImageSourceFactory *factory,
                      QString packagePath, QString imageName);

    virtual bool open();

    virtual bool copy(const QString &destPath);
private:
    QString m_packagePath;
};

#endif // SEVENZIMAGESOURCE_H
