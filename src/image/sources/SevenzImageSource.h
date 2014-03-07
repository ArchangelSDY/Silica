#ifndef SEVENZIMAGESOURCE_H
#define SEVENZIMAGESOURCE_H

#include <QBuffer>

#include "ImageSource.h"
#include "SevenzImageSourceFactory.h"

class Qt7zPackage;

class SevenzImageSource : public ImageSource
{
public:
    SevenzImageSource(QString packagePath, QString imageName,
                      SevenzImageSourceFactory *factory);

    virtual bool open();

    virtual bool copy(const QString &destPath);
private:
    QString m_packagePath;
    SevenzImageSourceFactory *m_factory;
};

#endif // SEVENZIMAGESOURCE_H
