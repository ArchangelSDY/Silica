#ifndef SEVENZIMAGESOURCEFACTORY_H
#define SEVENZIMAGESOURCEFACTORY_H

#include "ImageSourceFactory.h"

class SevenzImageSourceFactory : public ImageSourceFactory
{
    Q_OBJECT
public:
    QString name() const;
    QString fileNamePattern() const;
    QString urlScheme() const;
    ImageSource *create(const QUrl &url);
    ImageSource *create(const QString &path);
};

#endif // SEVENZIMAGESOURCEFACTORY_H
