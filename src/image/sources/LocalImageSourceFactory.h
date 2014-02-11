#ifndef LOCALIMAGESOURCEFACTORY_H
#define LOCALIMAGESOURCEFACTORY_H

#include "ImageSource.h"
#include "ImageSourceFactory.h"

class LocalImageSourceFactory : public ImageSourceFactory
{
    Q_OBJECT
public:
    QString name() const;
    QString fileNamePattern() const;
    QString urlScheme() const;
    ImageSource *create(const QUrl &url);
    ImageSource *create(const QString &path);
};

#endif // LOCALIMAGESOURCEFACTORY_H
