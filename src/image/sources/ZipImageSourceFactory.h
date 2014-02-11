#ifndef ZIPIMAGESOURCEFACTORY_H
#define ZIPIMAGESOURCEFACTORY_H

#include "ImageSourceFactory.h"

class ZipImageSourceFactory : public ImageSourceFactory
{
    Q_OBJECT
public:
    QString name() const;
    QString fileNamePattern() const;
    QString urlScheme() const;
    ImageSource *create(const QUrl &url);
    ImageSource *create(const QString &path);
};

#endif // ZIPIMAGESOURCEFACTORY_H
