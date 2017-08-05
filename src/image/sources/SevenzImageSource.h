#ifndef SEVENZIMAGESOURCE_H
#define SEVENZIMAGESOURCE_H

#include <QBuffer>

#include "image/ImageSource.h"

class Qt7zPackage;

class SevenzImageSource : public ImageSource
{
public:
    SevenzImageSource(ImageSourceFactory *factory,
                      QString packagePath,
                      QString imageName,
                      QString password);

    virtual bool open();
    virtual bool exists();

    virtual bool copy(const QString &destPath);
private:
    QString m_packagePath;
    QString m_password;
};

#endif // SEVENZIMAGESOURCE_H
